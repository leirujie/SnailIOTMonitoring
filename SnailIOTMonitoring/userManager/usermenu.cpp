#include "usermenu.h"
#include "ui_usermenu.h"
#include "loginpage.h"
#include "global.h"
#include <QCryptographicHash>
#include "validator.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialog>

UserMenu::UserMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserMenu),
    pageViewUserInfo(nullptr),     // 初始化查看个人信息页的指针
    pageUpdateInfo(nullptr),       // 初始化修改个人信息页的指针
    pageUpdatePassword(nullptr),    // 初始化修改密码页的指针
    pageDeviceInfo(nullptr),       // 初始化设备数据页的指针
    pageDeviceData(nullptr)
{
    ui->setupUi(this);
    // 获取数据库单例实例
    db = UserDatabase::getInstance();
    // 获取QStackedWidget对象，用于页面切换管理
    m_userStackedWidget = ui->widgetUserMenu;
    pageDeviceInfo = new UserDeviceManager;

    // 初始化每个页面的指针
    pageViewUserInfo = ui->pageViewUserInfo;
    pageUpdateInfo = ui->pageUpdateInfo;
    pageUpdatePassword = ui->pageUpdatePassword;
    pageDeviceData = ui->pageDeviceData;
    logDataPage = nullptr;  // 初始化logDataPage为nullptr

    // 添加页面到stackedWidget
    m_userStackedWidget->addWidget(ui->pageViewUserInfo);     // 添加查看个人信息页
    m_userStackedWidget->addWidget(ui->pageUpdateInfo);       // 修改个人信息页
    m_userStackedWidget->addWidget(ui->pageUpdatePassword);   // 重置密码页
    m_userStackedWidget->addWidget(ui->pageDeviceData);       // 添加设备数据页
    m_userStackedWidget->addWidget(pageDeviceInfo);       // 添加设备信息页
    m_userStackedWidget->addWidget(ui->pageUserLog);          // 用户日志页面

    connect(ui->btnUserInfo,&QPushButton::clicked,this,&UserMenu::on_btnUserViewInfoClicked);//用户基本信息页面

    connect(ui->btnUpdataUserInfo, &QPushButton::clicked, this, [=]() { //修改个人信息界面
        clearInputFields();
        m_userStackedWidget->setCurrentWidget(pageUpdateInfo);
    });
    connect(ui->btnUpdateComper,&QPushButton::clicked,this,&UserMenu::on_btnUpdateClicked);//完成修改用户信息

    connect(ui->btn_userUpdatePassword,&QPushButton::clicked,this,&UserMenu::on_btnUpdatePasswordClicked); // 重置密码
    connect(ui->btnResetPassword, &QPushButton::clicked, this, [=]() {
        m_userStackedWidget->setCurrentWidget(pageUpdatePassword);
    });

    connect(ui->btnCatDeviceInfo,&QPushButton::clicked,this, [=]() {  //设备信息页面
            m_userStackedWidget->setCurrentWidget(pageDeviceInfo);
    });

    //点击跳转页面
    connect(ui->btnCatDeviceData, &QPushButton::clicked, this, [this](){   // 连接进入数据管理页面
        m_userStackedWidget->setCurrentWidget(pageDeviceData);
        showDataManagerDialog();
    });

    connect(ui->btnCatUserLog, &QPushButton::clicked, this, [=]() {      //个人日志页面
        if (!logDataPage) {
            logDataPage = new logdata();
            m_userStackedWidget->addWidget(logDataPage);
        }
        m_userStackedWidget->setCurrentWidget(logDataPage);
    });

    connect(ui->btnBack,&QPushButton::clicked,this,[=](){emit switchPage(ACCOUNT_PWD_LOGIN_PAGE);});//返回登陆页面
}

void UserMenu::on_btnUserViewInfoClicked()
{
    // 清理动态创建的页面
    if (realtimeDataPage) {
        m_userStackedWidget->removeWidget(realtimeDataPage);
        delete realtimeDataPage;
        realtimeDataPage = nullptr;
    }

    if (historyDataPage) {
        m_userStackedWidget->removeWidget(historyDataPage);
        delete historyDataPage;
        historyDataPage = nullptr;
    }

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
        m_userStackedWidget->setCurrentWidget(pageViewUserInfo);
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

    // 进行格式验证
    if (!Validator::isValidEmail(newEmail)) {
        QMessageBox::warning(this, "错误", "请输入有效的邮箱地址！");
        return;
    }

    if (!Validator::isValidPhone(newPhone)) {
        QMessageBox::warning(this, "错误", "请输入有效的手机号！");
        return;
    }

    // 如果用户没有修改某个字段，使用当前数据库中的原值
    QString updatedEmail = newEmail.isEmpty()? m_currentUserInfo["email"] : newEmail;
    QString updatedPhone = newPhone.isEmpty()? m_currentUserInfo["phone"] : newPhone;
    QString updatedNickname = newNickname.isEmpty()? m_currentUserInfo["nickname"] : newNickname;

    // 调用更新函数，并根据返回值判断更新是否成功
    bool updateSuccess = db->updateUserInfo(m_username, m_currentUserInfo["password"], updatedEmail, updatedPhone, updatedNickname);
    if (updateSuccess) {
        QMessageBox::information(this, "成功", "用户信息更新成功！");
        LogManager::instance().logMessage(LogManager::INFO, "operation", "用户" + m_username + "更新了个人信息");
    } else {
        QMessageBox::warning(this, "错误", "用户信息更新失败，请检查输入的信息是否符合要求或联系管理员！");
    }
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

    // 检查旧密码长度是否符合要求
    if (oldPassword.length() < 8) {
        QMessageBox::warning(this, "错误", "旧密码长度至少应为8位，请重新输入。");
        return;
    }

    // 检查旧密码是否正确
    if (oldPasswordHash!= m_currentUserInfo["password"]) {
        QMessageBox::warning(this, "错误", "旧密码不正确，请重新输入。");
        return;
    }

    // 检查新密码与确认密码是否一致
    if (newPassword!= confirmPassword) {
        QMessageBox::warning(this, "错误", "两次输入的新密码不一致，请重新输入。");
        return;
    }

    // 检查新密码长度是否符合要求
    if (newPassword.length() < 8) {
        QMessageBox::warning(this, "错误", "新密码长度至少应为8位，请重新输入。");
        return;
    }

    // 对新密码进行SHA-256哈希
    QByteArray newPasswordHash = QCryptographicHash::hash(newPassword.toUtf8(), QCryptographicHash::Sha256).toHex();

    // 更新密码，并根据返回值判断更新是否成功
    bool updateSuccess = db->updateUserInfo(m_username, QString(newPasswordHash));
    if (updateSuccess) {
        // 清空密码输入框
        ui->lineEditOldPwd->clear();
        ui->lineEditNewPwd->clear();
        ui->lineEditAgainPwd->clear();

        QMessageBox::information(this, "提示", "密码修改成功！");
        LogManager::instance().logMessage(LogManager::INFO, "operation", "用户" + m_username + "修改了密码");
        emit switchPage(ACCOUNT_PWD_LOGIN_PAGE);
    } else {
        QMessageBox::warning(this, "错误", "密码修改失败，请检查输入的信息是否符合要求或联系管理员！");
    }
}

// 清空输入框
void UserMenu::clearInputFields()
{
    ui->lineEditEmail->clear();
    ui->lineEditPhone->clear();
    ui->lineEditNickname->clear();
}

void UserMenu::showDataManagerDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("选择数据类型");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QPushButton *btnRealtime = new QPushButton("查看实时数据", &dialog);
    QPushButton *btnHistorical = new QPushButton("查看历史数据", &dialog);

    layout->addWidget(btnRealtime);
    layout->addWidget(btnHistorical);

    connect(btnRealtime, &QPushButton::clicked, this, [this]() {
        // 移除之前可能存在的实时数据页面
        if (realtimeDataPage) {
            m_userStackedWidget->removeWidget(realtimeDataPage);
            delete realtimeDataPage;
            realtimeDataPage = nullptr;
        }

        // 创建并切换到实时数据页面
        realtimeDataPage = new RealTimeData();
        m_userStackedWidget->addWidget(realtimeDataPage);
        m_userStackedWidget->setCurrentWidget(realtimeDataPage);
    });

    connect(btnHistorical, &QPushButton::clicked, this, [this]() {
        // 移除之前可能存在的历史数据页面
        if (historyDataPage) {
            m_userStackedWidget->removeWidget(historyDataPage);
            delete historyDataPage;
            historyDataPage = nullptr;
        }

        // 创建并切换到历史数据页面
        historyDataPage = new HistoryTimeData();
        m_userStackedWidget->addWidget(historyDataPage);
        m_userStackedWidget->setCurrentWidget(historyDataPage);
    });

    dialog.exec();
}

UserMenu::~UserMenu()
{
    delete ui;
    delete pageUpdateInfo;
    delete pageViewUserInfo;
    delete realtimeDataPage;
    delete pageUpdatePassword;
    delete pageDeviceData;
    delete historyDataPage;
}

