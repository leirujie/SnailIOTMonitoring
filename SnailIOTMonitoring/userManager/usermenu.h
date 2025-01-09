#ifndef USERMENU_H
#define USERMENU_H

#include <QWidget>
#include "userManager/UserDatabase.h"
#include <FrameWork/centerwidget.h>
#include <QStackedWidget>

enum UserPageIndex {
    PAGE_USER_DEVICE_DATA = 0,//查看设备数据
    PAGE_USER_VIEW_INFO = 1,  //查看个人基本信息
    PAGE_USER_VIEW_LOG = 2,   //查看个人日志记录
    PAGE_USER_UPDATE_INFO = 3, //修改个人基本信息
    PAGE_USER_UPDATA_PASSWORD = 4 //修改密码
};


namespace Ui {
class UserMenu;
}

class UserMenu : public QWidget
{
    Q_OBJECT

public:
    explicit UserMenu(QWidget *parent = nullptr);
    ~UserMenu();

    QStackedWidget* getUserMenuStackedWidget() { return m_userStackedWidget; };  // 用于获取QStackedWidget对象

    void clearInputFields(); //清除输入框


signals:
    void switchPage(int targetPageIndex);  // 传递目标页面索引


private slots:
    void on_btnUserViewInfoClicked();    // 处理切换到个人基本信息页面
    void on_btnUpdateClicked();          // 处理完成修改个人信息槽函数
    void on_btnUpdatePasswordClicked();

private:
    Ui::UserMenu *ui;
    QStackedWidget *m_userStackedWidget;  // 用于管理AdminMenu中的页面切换
    UserDatabase *db;
};

#endif // USERMENU_H
