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
