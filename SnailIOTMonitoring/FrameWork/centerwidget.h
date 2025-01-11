#ifndef CENTERWIDGET_H
#define CENTERWIDGET_H

#include <QWidget>
#include <QStackedLayout>
#include "userManager/launchscrren.h"
#include "userManager/loginpage.h"
#include "userManager/registerpage.h"
#include "userManager/usermenu.h"
#include "userManager/adminmenu.h"

class LaunchScrren;
class LoginPage;
class RegisterPage;
class UserMenu;
class AdminMenu;

// 定义页面索引枚举
enum PageIndex {
    LAUNCH_SCREEN_PAGE = 0, // 启动页面
    ACCOUNT_PWD_LOGIN_PAGE = 1, // 登录界面
    REGISTER_PAGE = 2, // 注册页面
    Menu_User_PAGE = 3, //用户菜单
    Menu_ADMIN_PAGE = 4, //管理员菜单
    PAGE_USER_DEVICE_DATA = 5,
    PAGE_USER_DEVICE_INFO = 6
};

namespace Ui {
class centerWidget;
}

class centerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit centerWidget(QWidget *parent = nullptr);
    ~centerWidget();

signals:
    void sendMessageByCenter(QString);

private slots:
    void onSwitchPage(int targetPageIndex);  // 用于接收切换页面的信号

private:
    Ui::centerWidget *ui;
    QStackedLayout *m_QStackedLayout;
    LaunchScrren *launchscrren;//启动页
    LoginPage *loginPage;      //登录页
    RegisterPage *registerPage;//注册页
    AdminMenu *adminMenu;  //管理员菜单
    UserMenu *userMenu;    //用户菜单
};

#endif // CENTERWIDGET_H
