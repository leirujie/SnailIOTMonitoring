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

signals:
    void switchPage(int targetPageIndex);  // 传递目标页面索引

private slots:
    void onRegisClicked();  // 处理注册按钮点击事件的槽函数声明
    void onBackToLaunchClicked();  // 处理返回登录按钮点击事件的槽函数声明

private:
    Ui::RegisterPage *ui;
    UserDatabase *db;  // 创建Databas类的实例，用于数据库操作
};

#endif // REGISTERPAGE_H
