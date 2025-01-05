#include "loginpage.h"
#include "ui_loginpage.h"
#include "UserDatabase.h"  // 引入修改后的数据库操作类头文件

LoginPage::LoginPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginPage)
{
    ui->setupUi(this);
    // 初始化错误次数为 0
    errorCount = 0;
    // 创建数据库操作对象
    db = new UserDatabase();

    connect(ui->loginBtn, &QPushButton::clicked, this, &LoginPage::loginClicked);
    connect(ui->bakeToLauBtn, &QPushButton::clicked, this, &LoginPage::extiClicked);
    // 将光标设置在用户名输入框
    ui->usernameEdit->setFocus();
}

void LoginPage::loginClicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    QMap<QString, QString> userInfo = db->queryUserInfo(username);  // 从数据库查询该用户名对应的所有用户信息

    if (!userInfo.isEmpty() && userInfo["password"] == password) {
        QMessageBox::information(this, "成功", "登录成功", QMessageBox::Ok);
    } else {
        errorCount++;
        QMessageBox::warning(this, "错误", "用户名或密码错误，请重新输入！","");

        if (errorCount >= 3) {
            QMessageBox::critical(this, "错误", "错误次数超过三次，登录界面将关闭！","");
            close();
        }
    }
}

void LoginPage::extiClicked()
{
     emit switchPage(LAUNCH_SCREEN_PAGE);  // 发射信号，通知切换到
}

void LoginPage::accountClicked()
{
    emit switchPage(REGISTER_PAGE);  // 发射信号
}

LoginPage::~LoginPage()
{
    delete ui;
    delete db;  // 释放数据库操作对象内存
}
