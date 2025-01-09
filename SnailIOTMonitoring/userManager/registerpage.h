#ifndef REGISTERPAGE_H
#define REGISTERPAGE_H

#include <QWidget>
#include "UserDatabase.h"
#include "FrameWork/centerwidget.h"
#include <memory>

namespace Ui {
class RegisterPage;
}

class RegisterPage : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterPage(QWidget *parent = nullptr);
    ~RegisterPage();

    // 验证用户名格式是否合法的函数声明
    bool isValidUsername(const QString &username);
    // 验证密码格式是否合法的函数声明
    bool isValidPassword(const QString &password);
    // 验证邮箱格式是否合法的函数声明
    bool isValidEmail(const QString &email);
    // 验证手机号码格式是否合法的函数声明
    bool isValidPhone(const QString &phone);
    void clearInputRegister();  // 清空输入框

signals:
    void switchPage(int targetPageIndex);  // 传递目标页面索引

private slots:
    void onRegisClicked();  // 处理注册按钮点击事件的槽函数声明

private:
    Ui::RegisterPage *ui;
    UserDatabase *db;
};

#endif // REGISTERPAGE_H
