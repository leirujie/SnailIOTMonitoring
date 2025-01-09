#include "loginpage.h"
#include "ui_loginpage.h"
#include "UserDatabase.h"
#include <QCryptographicHash>
#include <QDebug>

LoginPage::LoginPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginPage)
{
    ui->setupUi(this);
    // 初始化错误次数为 0
    errorCount = 0;
    // 获取UserDatabase单例实例
    db = UserDatabase::getInstance(parent);

    connect(ui->loginBtn, &QPushButton::clicked, this, &LoginPage::loginClicked);
    connect(ui->bakeToLauBtn, &QPushButton::clicked, this, [=](){ emit switchPage(LAUNCH_SCREEN_PAGE);});
    // 将光标设置在用户名输入框
    ui->usernameEdit->setFocus();
}

void LoginPage::loginClicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    QString role;


    // 验证用户名
    if (username.length() < 3) {
        QMessageBox::warning(this, "错误", "用户名长度至少应为3位，请重新输入！");
        return;
    }

    // 验证密码
    if (password.length() < 6) {
        QMessageBox::warning(this, "错误", "密码长度至少应为6位，请重新输入！");
        return;
    }

    // 验证密码是否只包含数字和英文字
    QRegularExpression passwordRegex("^[a-zA-Z0-9]+$");
    if (!passwordRegex.match(password).hasMatch()) {
        QMessageBox::warning(this, "错误", "密码只能包含数字和英文字母，请重新输入！");
        return;
    }

    // 对输入的密码进行SHA-256加密处理
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    QString encryptedPassword = hash.toHex();  // 获取加密后的密码

    QMap<QString, QString> userInfo = db->queryUserInfo(username);  // 从数据库查询该用户名对应的所有用户信息

    if (!userInfo.isEmpty() && userInfo["password"] == encryptedPassword) {
        g_usernameMutex.lock();
        g_currentUsername = username;//全局变量保存当前登录用户
        g_usernameMutex.unlock();
        QMessageBox::information(this, "成功", "登录成功", QMessageBox::Ok);
        // 登录成功后根据用户角色（admin，user）进入对应的菜单
        role = userInfo["role"];
        if (role == "user") {
            emit switchPage(Menu_User_PAGE);
        } else {
            emit switchPage(Menu_ADMIN_PAGE);
        }

    } else {
        errorCount++;
        int remainingAttempts = 3 - errorCount;  // 计算剩余的登录次数

        if (remainingAttempts > 0) {
            QMessageBox::warning(this, "错误", QString("用户名或密码错误，请重新输入！\n剩余登录机会: %1").arg(remainingAttempts));
        } else {
            QMessageBox::critical(this, "错误", "错误次数超过三次，登录界面将关闭！");
            close();
        }
    }
}

void LoginPage::clearInputLogin() {
    if (ui) {
        ui->usernameEdit->clear();
        ui->passwordEdit->clear();
    } else {
        qDebug() << "UI is not initialized!";
    }
}


LoginPage::~LoginPage()
{
    delete ui;
}
