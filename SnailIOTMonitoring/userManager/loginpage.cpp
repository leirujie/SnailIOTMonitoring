#include "loginpage.h"
#include "ui_loginpage.h"
#include "UserDatabase.h"
#include <QCryptographicHash>
#include <QDebug>
#include "forgotpassworddialog.h"
#include "validator.h"

LoginPage::LoginPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginPage)
{
    ui->setupUi(this);
    // 初始化错误次数为 0
    errorCount = 0;
    // 获取UserDatabase单例实例
    db = UserDatabase::getInstance();

    connect(ui->loginBtn, &QPushButton::clicked, this, &LoginPage::loginClicked);
    connect(ui->bakeToLauBtn, &QPushButton::clicked, this, [=](){ emit switchPage(LAUNCH_SCREEN_PAGE);});
    connect(ui->btn_forgetPwd,&QPushButton::clicked,this,[](){
        ForgotPasswordDialog dialog;
        dialog.exec();
    });
    connect(ui->showPasswordCheckBox, &QCheckBox::stateChanged, this, &LoginPage::togglePasswordVisibility); // 连接复选框信号到槽
    // 将光标设置在用户名输入框
    ui->usernameEdit->setFocus();
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
}

// 切换密码显示模式
void LoginPage::togglePasswordVisibility(int state)
{
    if (state == Qt::Checked) {
        // 如果复选框被选中，显示密码
        ui->passwordEdit->setEchoMode(QLineEdit::Normal);
    } else {
        // 否则，隐藏密码
        ui->passwordEdit->setEchoMode(QLineEdit::Password);
    }
}

void LoginPage::loginClicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    QString role;


    //进行格式验证
    if (!Validator::isValidUsername(username)) {
        QMessageBox::warning(this, "错误", "用户名必须为4-20位的字母和数字组合！");
        return;
    }

    if (!Validator::isValidPassword(password)) {
        QMessageBox::warning(this, "错误", "密码至少包含8位的字母和数字！");
        return;
    }


    // 对输入的密码进行SHA-256加密处理
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    QString encryptedPassword = hash.toHex();  // 获取加密后的密码

    // 调用数据库接口查询用户信息，利用返回值判断查询是否成功
    QMap<QString, QString> userInfo = db->queryUserInfo(username);
    if (!userInfo.empty()) {
        // 查询成功，继续验证密码等后续操作
        if (userInfo["password"] == encryptedPassword) {
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
            // 密码不匹配情况处理
            handleLoginFailure();
        }
    } else {
        QMessageBox::warning(this, "错误", "查询用户信息失败，请检查网络或联系管理员！");
    }
}

void LoginPage::handleLoginFailure()
{
    errorCount++;
    int remainingAttempts = 3 - errorCount;  // 计算剩余的登录次数

    if (remainingAttempts > 0) {
        QMessageBox::warning(this, "错误", QString("用户名或密码错误，请重新输入！\n剩余登录机会: %1").arg(remainingAttempts));
    } else {
        QMessageBox::critical(this, "错误", "错误次数超过三次，登录界面将关闭！");
        emit switchPage(LAUNCH_SCREEN_PAGE);
        clearInputLogin();  // 登录失败达到最大次数，清空输入框
    }
}

void LoginPage::clearInputLogin() {
    if (ui) {
        ui->usernameEdit->clear();
        ui->passwordEdit->clear();
    }
}


LoginPage::~LoginPage()
{
    delete ui;
}
