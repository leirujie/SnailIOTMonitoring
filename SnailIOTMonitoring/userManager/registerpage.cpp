#include "registerpage.h"
#include "ui_registerpage.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include "UserDatabase.h"
#include <QDebug>
#include "validator.h"

RegisterPage::RegisterPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterPage)
{
    ui->setupUi(this);
    // 获取UserDatabase单例实例
    db = UserDatabase::getInstance();
    connect(ui->regis_btn, &QPushButton::clicked, this, &RegisterPage::onRegisClicked);
    connect(ui->backtologin_btn, &QPushButton::clicked, this, [=](){ emit switchPage(LAUNCH_SCREEN_PAGE);});
}


void RegisterPage::onRegisClicked()
{
    QString username = ui->regisUser_text->text();
    QString password = ui->regisPwd_text->text();
    QString confirmPassword = ui->regisPwdTwo_text->text();
    QString email = ui->regisEmail_text->text();
    QString phone = ui->regisPhone_text->text();
    QString nickname = ui->regisNick_text->text();
    QString role = "user";

    // 验证两次输入的密码是否一致
    if (password!= confirmPassword) {
        QMessageBox::warning(this, "错误", "两次输入的密码不一致，请重新输入！");
        return;
    }

    // 使用 Validator 类进行格式验证
    if (!Validator::isValidUsername(username)) {
        QMessageBox::warning(this, "错误", "用户名格式不正确，只能包含字母、数字和下划线，且长度至少为4位，请重新输入！");
        return;
    }

    if (!Validator::isValidPassword(password)) {
        QMessageBox::warning(this, "错误", "密码格式不正确，长度至少应为8位，包含字母和数字，请重新输入！");
        return;
    }

    if (!Validator::isValidEmail(email)) {
        QMessageBox::warning(this, "错误", "邮箱格式不正确，请重新输入！");
        return;
    }

    if (!Validator::isValidPhone(phone)) {
        QMessageBox::warning(this, "错误", "手机号码格式不正确，请重新输入！");
        return;
    }

    // 利用数据库接口检查用户名、手机号、邮箱是否已存在
    bool userExists = db->checkUserExistence(username, phone, email);
    if (userExists) {
        QMap<QString, QString> existingUserInfo = db->queryUserInfo(username);
        if (username == existingUserInfo["username"]) {
            QMessageBox::warning(this, "错误", "该用户名已存在，请更换用户名重新注册！");
        } else if (phone == existingUserInfo["phone"]) {
            QMessageBox::warning(this, "错误", "该手机号码已被注册，请更换手机号码重新注册！");
        } else if (email == existingUserInfo["email"]) {
            QMessageBox::warning(this, "错误", "该邮箱已被注册，请更换邮箱重新注册！");
        }
        return;
    }

    // 对密码进行SHA256加密处理
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    QString encryptedPassword = hash.toHex();

    // 调用数据库保存接口，根据返回值判断是否保存成功
    bool saveSuccess = db->saveUserInfo(username, encryptedPassword, email, phone, nickname, role);
    if (saveSuccess) {
        QMessageBox::information(this, "成功", "注册成功！");
        LogManager::instance().logMessage(LogManager::INFO, "operation", "用户" + username + "注册了系统");
        // 发射信号，传递页面索引，通知外部切换回启动界面
        emit switchPage(LAUNCH_SCREEN_PAGE);
    } else {
        // 如果保存失败，进一步判断是否是数据库连接等其他问题导致
        if (!db->openDatabase()) {
            QMessageBox::warning(this, "错误", "数据库连接失败，无法保存用户信息");
            LogManager::instance().logMessage(LogManager::ERROR, "exception", "用户数据库连接失败");
            qDebug() << "数据库连接失败，无法保存用户信息";
        } else {
            QMessageBox::warning(this, "错误", "注册信息保存失败，请稍后重试！");
        }
    }
}

void RegisterPage::clearInputRegister()
{
    ui->regisUser_text->clear();
    ui->regisPwd_text->clear();
    ui->regisPwdTwo_text->clear();
    ui->regisEmail_text->clear();
    ui->regisPhone_text->clear();
    ui->regisNick_text->clear();
}

RegisterPage::~RegisterPage()
{
    delete ui;
}
