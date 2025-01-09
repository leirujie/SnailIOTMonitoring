#include "adduserdialog.h"
#include "ui_adduserdialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QCryptographicHash>

AddUserDialog::AddUserDialog(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::AddUserDialog)
{
    ui->setupUi(this);
    // 获取UserDatabase单例实例
    db = UserDatabase::getInstance(parent);
    // 设置角色选择框的选项
    ui->roleComboBox->addItem("user");
    ui->roleComboBox->addItem("admin");

    connect(ui->addButton, &QPushButton::clicked, this, &AddUserDialog::on_addButton_clicked);
}

QString AddUserDialog::username() const
{
    return ui->usernameEdit->text();  // 获取用户名
}

QString AddUserDialog::password() const
{
    return ui->passwordEdit->text();  // 获取密码
}

QString AddUserDialog::email() const
{
    return ui->emailEdit->text();  // 获取邮箱
}

QString AddUserDialog::phone() const
{
    return ui->phoneEdit->text();  // 获取电话
}

QString AddUserDialog::role() const
{
    return ui->roleComboBox->currentText();  // 获取角色
}
void AddUserDialog::on_addButton_clicked()
{
    // 获取用户输入的值
    QString username = this->username();
    QString password = this->password();
    QString email = this->email();
    QString phone = this->phone();
    QString role = this->role();
    qDebug() << "Username entered: " << username;

    // 数据验证
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "错误", "用户名和密码不能为空！");
        return;
    }

    // 对密码进行SHA-256哈希加密
    QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
    QString encryptedPassword = QString(passwordHash);

    QString defaultNickname = "默认昵称";
    // 调用数据库接口保存用户信息，传递加密后的密码
    db->saveUserInfo(username, encryptedPassword, email, phone, defaultNickname, role);

    QMessageBox::information(this, "成功", "用户添加成功！");

    accept();  // 关闭对话框
}

AddUserDialog::~AddUserDialog()
{
    delete ui;
}
