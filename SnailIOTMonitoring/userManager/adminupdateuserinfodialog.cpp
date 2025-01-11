#include "adminupdateuserinfodialog.h"
#include "ui_adminupdateuserinfodialog.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include "validator.h"

adminUpdateUserInfoDialog::adminUpdateUserInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::adminUpdateUserInfoDialog)
{
    ui->setupUi(this);

    // 获取UserDatabase单例实例
    db = UserDatabase::getInstance();
    // 设置角色选择框的选项
    ui->roleComboBox->addItem("user");
    ui->roleComboBox->addItem("admin");

    // 隐藏修改信息的部分，等待查询用户名后再显示
    ui->passwordEdit->setEnabled(false);
    ui->emailEdit->setEnabled(false);
    ui->phoneEdit->setEnabled(false);
    ui->roleComboBox->setEnabled(false);
    ui->updateButton->setEnabled(false);
}


void adminUpdateUserInfoDialog::on_queryButton_clicked()
{
    // 获取输入的用户名
    QString username = ui->usernameEdit->text();

    // 查询用户信息
    QMap<QString, QString> userInfo = db->queryUserInfo(username);

    // 如果未找到用户，提示错误
    if (userInfo.isEmpty()) {
        QMessageBox::warning(this, "错误", "未找到指定用户名的用户，请确认输入是否正确！");
        return;
    }

    // 显示查询到的用户信息
    ui->passwordEdit->setEnabled(true);
    ui->emailEdit->setEnabled(true);
    ui->phoneEdit->setEnabled(true);
    ui->roleComboBox->setEnabled(true);
    ui->updateButton->setEnabled(true);

    ui->passwordEdit->setText(userInfo["password"]);
    ui->emailEdit->setText(userInfo["email"]);
    ui->phoneEdit->setText(userInfo["phone"]);
    ui->roleComboBox->setCurrentText(userInfo["role"]);
}

void adminUpdateUserInfoDialog::on_updateButton_clicked()
{
    QString username =  ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    QString email =  ui->emailEdit->text();
    QString phone = ui->phoneEdit->text();  // 获取电话
    QString role = ui->roleComboBox->currentText();  // 获取角色

    // 对新密码进行SHA-256哈希加密
    if (!password.isEmpty()) {
        QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
        password = QString(passwordHash);
    }

    //进行格式验证
    if (!Validator::isValidUsername(username)) {
        QMessageBox::warning(this, "错误", "用户名必须为4-20位的字母和数字组合！");
        return;
    }

    if (!Validator::isValidPassword(password)) {
        QMessageBox::warning(this, "错误", "密码至少包含8位的字母和数字！");
        return;
    }
      if (!Validator::isValidEmail(email)) {
          QMessageBox::warning(this, "错误", "请输入有效的邮箱地址！");
          return;
      }

      if (!Validator::isValidPhone(phone)) {
          QMessageBox::warning(this, "错误", "请输入有效的手机号！");
          return;
      }

    // 获取当前昵称
    QString currentNickname = db->getUserNickname(username);
    if (currentNickname.isEmpty()) {
        QMessageBox::warning(this, "错误", "无法获取当前用户昵称，更新失败，请检查用户名是否正确或联系管理员。");
        return;
    }

    // 调用数据库接口更新用户信息，不修改用户昵称，传递当前昵称保持不变
    bool updateSuccess = db->updateUserInfo(username, password, email, phone, currentNickname, role);
    if (updateSuccess) {
        QMessageBox::information(this, "成功", "用户信息更新成功！");
        accept();  // 关闭对话框
    }  else {
        QMessageBox::warning(this, "错误", "用户信息更新失败，请检查输入的信息是否符合要求或联系管理员！");
    }
}

adminUpdateUserInfoDialog::~adminUpdateUserInfoDialog()
{
    delete ui;
}
