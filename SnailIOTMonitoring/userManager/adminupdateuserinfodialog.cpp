#include "adminupdateuserinfodialog.h"
#include "ui_adminupdateuserinfodialog.h"
#include <QMessageBox>
#include <QCryptographicHash>

adminUpdateUserInfoDialog::adminUpdateUserInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::adminUpdateUserInfoDialog)
{
    ui->setupUi(this);

    // 获取UserDatabase单例实例
    db = UserDatabase::getInstance(parent);
    // 设置角色选择框的选项
    ui->roleComboBox->addItem("user");
    ui->roleComboBox->addItem("admin");

    // 连接更新按钮的点击信号
    connect(ui->updateButton, &QPushButton::clicked, this, &adminUpdateUserInfoDialog::on_updateButton_clicked);

    // 隐藏修改信息的部分，等待查询用户名后再显示
    ui->passwordEdit->setEnabled(false);
    ui->emailEdit->setEnabled(false);
    ui->phoneEdit->setEnabled(false);
    ui->roleComboBox->setEnabled(false);
    ui->updateButton->setEnabled(false);
}

QString adminUpdateUserInfoDialog::username() const
{
    return ui->usernameEdit->text();  // 获取用户名
}

QString adminUpdateUserInfoDialog::password() const
{
    return ui->passwordEdit->text();  // 获取密码
}

QString adminUpdateUserInfoDialog::email() const
{
    return ui->emailEdit->text();  // 获取邮箱
}

QString adminUpdateUserInfoDialog::phone() const
{
    return ui->phoneEdit->text();  // 获取电话
}

QString adminUpdateUserInfoDialog::role() const
{
    return ui->roleComboBox->currentText();  // 获取角色
}

void adminUpdateUserInfoDialog::on_queryButton_clicked()
{
    // 获取输入的用户名
    QString username = this->username();

    // 数据验证
    if (username.isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入用户名！");
        return;
    }

    // 查询用户信息
    QMap<QString, QString> userInfo = db->queryUserInfo(username);

    // 如果未找到用户，提示错误
    if (userInfo.isEmpty()) {
        QMessageBox::warning(this, "错误", "未找到指定用户名的用户！");
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
    QString username = this->username();
    QString password = this->password();
    QString email = this->email();
    QString phone = this->phone();
    QString role = this->role();

    // 对新密码进行SHA-256哈希加密（如果密码有修改的情况）
    if (!password.isEmpty()) {
        QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
        password = QString(passwordHash);
    }

    // 调用数据库接口更新用户信息，传递加密后的密码（如果有修改）
    db->updateUserInfo(username, password, email, phone, "", role);

    QMessageBox::information(this, "成功", "用户信息更新成功！");
    accept();  // 关闭对话框
}

adminUpdateUserInfoDialog::~adminUpdateUserInfoDialog()
{
    delete ui;
}
