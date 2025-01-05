#include "registerpage.h"
#include "ui_registerpage.h"
#include <QMessageBox>
#include "UserDatabase.h"  // 引入修改后的数据库操作类头文件

RegisterPage::RegisterPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterPage)
{
    ui->setupUi(this);
    // 创建数据库操作对象
    db = new UserDatabase();
    // 连接注册按钮的点击信号到对应的槽函数
    connect(ui->regis_btn, &QPushButton::clicked, this, &RegisterPage::onRegisClicked);
    // 连接返回登录按钮的点击信号到对应的槽函数
    connect(ui->backtologin_btn, &QPushButton::clicked, this, &RegisterPage::onBackToLaunchClicked);
}

void RegisterPage::onRegisClicked()
{
    QString username = ui->regisUser_text->text();
    QString password = ui->regisPwd_text->text();
    QString confirmPassword = ui->regisPwdTwo_text->text();
    QString email = ui->regisEmail_text->text();  // 获取输入的邮箱
    QString phone = ui->regisPhone_text->text();  // 获取输入的手机号
    QString nickname = ui->regisNick_text->text();  // 获取输入的昵称
    QString role = "user";  // 默认为普通用户角色

    // 验证两次输入的密码是否一致
    if (password!= confirmPassword) {
        // 弹出提示框告知用户密码不一致
        QMessageBox::warning(this, "错误", "两次输入的密码不一致，请重新输入！","");
        return;
    }

    // 先查询数据库中是否已存在该用户名
    QMap<QString, QString> existingUserInfo = db->queryUserInfo(username);
    if (!existingUserInfo.isEmpty()) {
        // 如果查询到用户信息，说明用户已存在，弹出提示框告知用户
        QMessageBox::warning(this, "错误", "该用户名已存在，请更换用户名重新注册！","");
        return;
    }

    // 如果用户名不存在，执行注册操作，将用户信息保存到数据库
    db->saveUserInfo(username, password, email, phone, nickname, role);
    // 弹出提示框告知用户注册成功
    QMessageBox::information(this, "成功", "注册成功！","");
    // 发射信号，传递页面索引，通知外部切换回启动界面
    emit switchPage(LAUNCH_SCREEN_PAGE);
}

void RegisterPage::onBackToLaunchClicked()
{
    emit switchPage(LAUNCH_SCREEN_PAGE);
}

RegisterPage::~RegisterPage()
{
    delete ui;
    delete db;  // 释放数据库操作对象内存
}
