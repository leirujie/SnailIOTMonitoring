#include "adduserdialog.h"
#include "ui_adduserdialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QCryptographicHash>
#include "validator.h"

AddUserDialog::AddUserDialog(QWidget  *parent):
    QDialog(parent),
    ui(new Ui::AddUserDialog)
{
    ui->setupUi(this);
    // 获取UserDatabase单例实例
    db = UserDatabase::getInstance();
    // 设置角色选择框的选项
    ui->roleComboBox->addItem("user");
    ui->roleComboBox->addItem("admin");

    connect(ui->addButton, &QPushButton::clicked, this, &AddUserDialog::on_addButtonClicked);
}
void AddUserDialog::on_addButtonClicked()
{
    // 添加调试信息以确认槽函数被调用的次数
    qDebug() << "AddUserDialog::on_addButton_clicked() called";

    // 禁用添加按钮，防止多次点击
    ui->addButton->setEnabled(false);

    // 获取用户输入的值
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text();
    QString email = ui->emailEdit->text().trimmed();
    QString phone = ui->phoneEdit->text().trimmed();
    QString role = ui->roleComboBox->currentText();

    // 进行格式验证
    if (!Validator::isValidUsername(username)) {
        QMessageBox::warning(this, "错误", "用户名必须为4-20位的字母和数字组合！");
        ui->addButton->setEnabled(true); // 重新启用按钮
        return;
    }

    if (!Validator::isValidPassword(password)) {
        QMessageBox::warning(this, "错误", "密码至少包含8位的字母和数字！");
        ui->addButton->setEnabled(true); // 重新启用按钮
        return;
    }

    if (!Validator::isValidEmail(email)) {
        QMessageBox::warning(this, "错误", "无效的邮箱格式！");
        ui->addButton->setEnabled(true); // 重新启用按钮
        return;
    }

    if (!Validator::isValidPhone(phone)) {
        QMessageBox::warning(this, "错误", "无效的手机号格式！");
        ui->addButton->setEnabled(true); // 重新启用按钮
        return;
    }

    // 对密码进行SHA-256哈希加密
    QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
    QString encryptedPassword = QString(passwordHash);

    QString defaultNickname = "userNickname";

    // 分别检查用户名、电话和邮箱是否存在
    bool usernameExists = db->checkUsernameExists(username);
    bool phoneExists = db->checkPhoneExists(phone);
    bool emailExists = db->checkEmailExists(email);

    if (usernameExists || phoneExists || emailExists) {
        QString errorMsg;
        if (usernameExists) {
            errorMsg += "该用户名已存在，请更换用户名重新添加！\n";
        }
        if (phoneExists) {
            errorMsg += "该手机号已被注册，请更换手机号重新添加！\n";
        }
        if (emailExists) {
            errorMsg += "该邮箱已被注册，请更换邮箱重新添加！\n";
        }
        QMessageBox::warning(this, "错误", errorMsg.trimmed());
        ui->addButton->setEnabled(true); // 重新启用按钮
        return;
    }

    // 调用数据库接口保存用户信息，传递加密后的密码，并根据返回值判断是否保存成功
    bool saveSuccess = db->saveUserInfo(username, encryptedPassword, email, phone, defaultNickname, role);
    if (saveSuccess) {
        QMessageBox::information(this, "成功", "用户添加成功！");
        accept();  // 关闭对话框
    } else {
        // 如果保存失败
        QMap<QString, QString> testQuery = db->queryUserInfo(username);
        if (testQuery.empty()) {
            QMessageBox::warning(this, "错误", "数据库连接可能出现问题，无法添加用户，请稍后重试！");
            qDebug() << "可能数据库连接异常，添加用户失败";
        } else {
            QMessageBox::warning(this, "错误", "用户添加失败，请稍后重试！");
        }
        ui->addButton->setEnabled(true); // 重新启用按钮
    }
}

AddUserDialog::~AddUserDialog()
{
    delete ui;
}

