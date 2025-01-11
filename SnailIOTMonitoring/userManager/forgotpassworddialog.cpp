#include "forgotpassworddialog.h"
#include "ui_forgotpassworddialog.h"
#include <QRegularExpression>
#include <QMessageBox>
#include "UserDatabase.h"

ForgotPasswordDialog::ForgotPasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ForgotPasswordDialog)
{
    ui->setupUi(this);
    // 获取数据库单例实例
    db = UserDatabase::getInstance();

    connect(ui->pushButton, &QPushButton::clicked, this, &ForgotPasswordDialog::onRecoverButtonClicked);
}

// 验证邮箱格式是否合法
bool ForgotPasswordDialog::isValidEmail(const QString &email)
{
    QRegularExpression emailRegex("^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z0-9-.]+$");
    return emailRegex.match(email).hasMatch();
}

// 验证手机号码格式是否合法
bool ForgotPasswordDialog::isValidPhone(const QString &phone)
{
    QRegularExpression phoneRegex("^1[3-9]\\d{9}$");
    return phoneRegex.match(phone).hasMatch();
}

void ForgotPasswordDialog::onRecoverButtonClicked()
{
    QString input = ui->lineEdit->text();  // 获取用户输入的邮箱或手机号
    QString recoveryType = ui->comboBox->currentText();  // 获取选择的找回方式（邮箱或手机）

    // 验证输入是否为空
    if (input.isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入邮箱或手机号！");
        return;
    }

    // 根据选择的找回方式进行相应验证和查找用户操作
    if (recoveryType == "邮箱") {
        if (!isValidEmail(input)) {
            QMessageBox::warning(this, "错误", "请输入有效的邮箱地址！");
            return;
        }
        // 查询数据库中是否存在该邮箱对应的用户
        QList<QMap<QString, QString>> allUsers = db->queryAllUserInfo();
        bool userFound = false;
        for (const QMap<QString, QString>& userInfo : allUsers) {
            if (userInfo["email"] == input) {
                userFound = true;
                break;
            }
        }
        if (!userFound) {
            QMessageBox::warning(this, "错误", "该邮箱未注册，请检查输入是否正确！");
            return;
        }

        QMessageBox::information(this, "提示", "已向您的邮箱发送重置密码链接，请查收邮件并按照提示操作！");
    } else if (recoveryType == "手机") {
        if (!isValidPhone(input)) {
            QMessageBox::warning(this, "错误", "请输入有效的手机号！");
            return;
        }
        // 查询数据库中是否存在该手机号对应的用户
        QList<QMap<QString, QString>> allUsers = db->queryAllUserInfo();
        bool userFound = false;
        for (const QMap<QString, QString>& userInfo : allUsers) {
            if (userInfo["phone"] == input) {
                userFound = true;
                break;
            }
        }
        if (!userFound) {
            QMessageBox::warning(this, "错误", "该手机号未注册，请检查输入是否正确！");
            return;
        }
        QMessageBox::information(this, "提示", "已向您的手机号发送验证码，请输入验证码重置密码！");
    } else {
        QMessageBox::warning(this, "错误", "请选择正确的找回方式（邮箱或手机）！");
        return;
    }
}


ForgotPasswordDialog::~ForgotPasswordDialog()
{
    delete ui;
}
