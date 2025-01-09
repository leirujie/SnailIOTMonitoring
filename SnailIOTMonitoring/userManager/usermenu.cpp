#include "usermenu.h"
#include "ui_usermenu.h"
#include "loginpage.h"
#include "global.h"
#include <QCryptographicHash>

UserMenu::UserMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserMenu)
{
    ui->setupUi(this);
    // 获取数据库单例实例
    db = UserDatabase::getInstance(parent);

    // 获取QStackedWidget对象，用于页面切换管理
    m_userStackedWidget = ui->widgetUserMenu;
    // 添加页面到stackedWidget
    m_userStackedWidget->addWidget(ui->pageDeviceData);  //添加设备数据页
    m_userStackedWidget->addWidget(ui->pageViewUserInfo); //添加查看个人信息页
    m_userStackedWidget->addWidget(ui->pageUserLog);     //用户日志页面
    m_userStackedWidget->addWidget(ui->pageUpdateInfo);  //修改个人信息页
    m_userStackedWidget->addWidget(ui->pageUpdatePassword);


    //点击跳转页面
    connect(ui->btnCatDeviceData,&QPushButton::clicked,this, [=]() {  //设备数据页面
        m_userStackedWidget->setCurrentIndex(PAGE_USER_DEVICE_DATA);
    });
    connect(ui->btnUserInfo,&QPushButton::clicked,this,&UserMenu::on_btnUserViewInfoClicked);//用户基本信息页面
    connect(ui->btnCatUserLog, &QPushButton::clicked, this, [=]() {      //个人日志页面
        m_userStackedWidget->setCurrentIndex(PAGE_USER_VIEW_LOG);
    });
    connect(ui->btnUpdataUserInfo, &QPushButton::clicked, this, [=]() { //修改个人信息界面
        clearInputFields();
        m_userStackedWidget->setCurrentIndex(PAGE_USER_UPDATE_INFO);
    });

    // 重置密码按钮点击事件
    connect(ui->btnResetPassword, &QPushButton::clicked, this, [=]() {
        m_userStackedWidget->setCurrentIndex(PAGE_USER_UPDATA_PASSWORD);
    });

    connect(ui->btnUpdateComper,&QPushButton::clicked,this,&UserMenu::on_btnUpdateClicked);//完成修改用户信息
    connect(ui->btn_userUpdatePassword,&QPushButton::clicked,this,&UserMenu::on_btnUpdatePasswordClicked);
    connect(ui->btnBack,&QPushButton::clicked,this,[=](){emit switchPage(ACCOUNT_PWD_LOGIN_PAGE);});//返回登陆页面
}

void UserMenu::on_btnUserViewInfoClicked()
{
    QString m_username;
    // 获取当前用户
    g_usernameMutex.lock();
    m_username = g_currentUsername; // 从全局变量获取用户名
    g_usernameMutex.unlock();

    QMap<QString, QString> m_currentUserInfo = db->queryUserInfo(m_username);
    if (!m_currentUserInfo.isEmpty()) {
        ui->labelUsername->setText(m_currentUserInfo["username"]);
        ui->labelEmail->setText(m_currentUserInfo["email"]);
        ui->labelPhone->setText(m_currentUserInfo["phone"]);
        ui->labelNickname->setText(m_currentUserInfo["nickname"]);
        m_userStackedWidget->setCurrentIndex(PAGE_USER_VIEW_INFO);
    } else {
        QMessageBox::warning(this, "错误", "未能获取到用户信息，请重新登录");
    }
}

//修改用户信息
void UserMenu::on_btnUpdateClicked()
{
    QString m_username;
    // 获取当前用户
    g_usernameMutex.lock();
    m_username = g_currentUsername; // 从全局变量获取用户名
    g_usernameMutex.unlock();


    QMap<QString, QString> m_currentUserInfo = db->queryUserInfo(m_username);

    // 获取新的输入值
    QString newEmail = ui->lineEditEmail->text();
    QString newPhone = ui->lineEditPhone->text();
    QString newNickname = ui->lineEditNickname->text();

    // 如果用户没有修改某个字段，使用当前数据库中的原值
    QString updatedEmail = newEmail.isEmpty() ? m_currentUserInfo["email"] : newEmail;
    QString updatedPhone = newPhone.isEmpty() ? m_currentUserInfo["phone"] : newPhone;
    QString updatedNickname = newNickname.isEmpty() ? m_currentUserInfo["nickname"] : newNickname;


    // 调用更新函数
    db->updateUserInfo(m_username, m_currentUserInfo["password"], updatedEmail, updatedPhone, updatedNickname);
}


void UserMenu::on_btnUpdatePasswordClicked()
{
    QString m_username;
    // 获取当前用户
    g_usernameMutex.lock();
    m_username = g_currentUsername; // 从全局变量获取用户名
    g_usernameMutex.unlock();

    QMap<QString, QString> m_currentUserInfo = db->queryUserInfo(m_username);

    // 获取用户输入的值
    QString oldPassword = ui->lineEditOldPwd->text();
    QString newPassword = ui->lineEditNewPwd->text();
    QString confirmPassword = ui->lineEditAgainPwd->text();

    // 对输入的旧密码进行SHA-256哈希
    QByteArray oldPasswordHash = QCryptographicHash::hash(oldPassword.toUtf8(), QCryptographicHash::Sha256).toHex();

    // 检查旧密码是否正确
    if (oldPasswordHash != m_currentUserInfo["password"]) {
        QMessageBox::warning(this, "错误", "旧密码不正确，请重新输入。");
        return;
    }

    // 检查新密码与确认密码是否一致
    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, "错误", "两次输入的新密码不一致，请重新输入。");
        return;
    }

    // 检查新密码长度是否符合要求（例如至少8位）
    if (newPassword.length() < 8) {
        QMessageBox::warning(this, "错误", "新密码长度至少应为8位，请重新输入。");
        return;
    }

    // 对新密码进行SHA-256哈希
    QByteArray newPasswordHash = QCryptographicHash::hash(newPassword.toUtf8(), QCryptographicHash::Sha256).toHex();

    // 更新密码
    bool updateSuccess = db->updateUserInfo(m_username, QString(newPasswordHash));

    if (updateSuccess) {
        // 清空密码输入框
        ui->lineEditOldPwd->clear();
        ui->lineEditNewPwd->clear();
        ui->lineEditAgainPwd->clear();

        QMessageBox::information(this, "提示", "密码修改成功！");
        emit switchPage(ACCOUNT_PWD_LOGIN_PAGE);
    } else {
        QMessageBox::warning(this, "错误", "密码修改失败，请稍后再试。");
    }
}


// 清空输入框
void UserMenu::clearInputFields()
{
    ui->lineEditEmail->clear();
    ui->lineEditPhone->clear();
    ui->lineEditNickname->clear();
}

UserMenu::~UserMenu()
{
    delete ui;
}

