#include "launchscrren.h"
#include "ui_launchscrren.h"
#include <QFile>

LaunchScrren::LaunchScrren(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LaunchScrren)
{
    ui->setupUi(this);
    connect(ui->userPwdLogin_btn, &QPushButton::clicked, this, [=](){
        emit switchPage(ACCOUNT_PWD_LOGIN_PAGE);  // 传递页面索引，切换到登录页面
    });
    connect(ui->register_btn,&QPushButton::clicked, this,[=](){
        emit switchPage(REGISTER_PAGE);  //传递页面索引，切换到注册页面
    });
    connect(ui->register_btn, &QPushButton::clicked, this, [=](){emit sendMsg("注册");});
}

LaunchScrren::~LaunchScrren()
{
    delete ui;
}

