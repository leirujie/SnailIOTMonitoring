#include "registerpage.h"
#include "ui_registerpage.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include "UserDatabase.h"

RegisterPage::RegisterPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterPage)
{
    ui->setupUi(this);
    // 获取UserDatabase单例实例
    db = UserDatabase::getInstance();
    // 连接注册按钮的点击信号到对应的槽函数
    connect(ui->regis_btn, &QPushButton::clicked, this, &RegisterPage::onRegisClicked);
    // 连接返回登录按钮的点击信号到对应的槽函数
    connect(ui->backtologin_btn, &QPushButton::clicked, this, [=](){ emit switchPage(LAUNCH_SCREEN_PAGE);});
}

// 验证用户名格式是否合法（只能包含字母、数字和下划线，长度至少3位)
bool RegisterPage::isValidUsername(const QString &username)
{
    QRegularExpression usernameRegex("^[a-zA-Z0-9_]{3,}$");
    return usernameRegex.match(username).hasMatch();
}

// 验证密码格式是否合法（限制为长度至少8位)
bool RegisterPage::isValidPassword(const QString &password)
{
    return password.length() >= 8;
}

// 验证邮箱格式是否合法
bool RegisterPage::isValidEmail(const QString &email)
{
    QRegularExpression emailRegex("^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z0-9-.]+$");
    return emailRegex.match(email).hasMatch();
}

// 验证手机号码格式是否合法
bool RegisterPage::isValidPhone(const QString &phone)
{
    if (phone.length()!= 11) {
        return false;
    }
    for (int i = 0; i < phone.length(); ++i) {
        if (!phone.at(i).isDigit()) {
            return false;
        }
    }
    return true;
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
        QMessageBox::warning(this, "错误", "两次输入的密码不一致，请重新输入！");
        return;
    }

    // 验证用户名格式
    if (!isValidUsername(username)) {
        QMessageBox::warning(this, "错误", "用户名格式不正确，只能包含字母、数字和下划线，且长度至少为3位，请重新输入！");
        return;
    }

    // 验证密码格式
    if (!isValidPassword(password)) {
        QMessageBox::warning(this, "错误", "密码格式不正确，长度至少应为8位，请重新输入！");
        return;
    }

    // 验证邮箱格式
    if (!isValidEmail(email)) {
        QMessageBox::warning(this, "错误", "邮箱格式不正确，请重新输入！");
        return;
    }

    // 验证手机号码格式
    if (!isValidPhone(phone)) {
        QMessageBox::warning(this, "错误", "手机号码格式不正确，请重新输入！");
        return;
    }

    // 先查询数据库中是否已存在该用户名
    QMap<QString, QString> existingUserInfo = db->queryUserInfo(username);
    if (!existingUserInfo.isEmpty()) {
        // 如果查询到用户信息，说明用户已存在，弹出提示框告知用户
        QMessageBox::warning(this, "错误", "该用户名已存在，请更换用户名重新注册！");
        return;
    }

    // 对密码进行SHA256加密处理
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    QString encryptedPassword = hash.toHex();

    // 如果用户名不存在，执行注册操作，将用户信息保存到数据库
    db->saveUserInfo(username, encryptedPassword, email, phone, nickname, role);
    // 弹出提示框告知用户注册成功
    QMessageBox::information(this, "成功", "注册成功！");
    // 发射信号，传递页面索引，通知外部切换回启动界面
    emit switchPage(LAUNCH_SCREEN_PAGE);
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
