#ifndef CENTERWIDGET_H
#define CENTERWIDGET_H

#include <QWidget>
#include <QStackedLayout>
#include "userManager/launchscrren.h"
#include "userManager/loginpage.h"
#include "userManager/registerpage.h"

class LaunchScrren;
class LoginPage;
class RegisterPage;

// 定义页面索引枚举
enum PageIndex {
    LAUNCH_SCREEN_PAGE = 0, // 启动页面
    ACCOUNT_PWD_LOGIN_PAGE = 1, // 登录界面
    REGISTER_PAGE = 2, // 注册页面
    DEVICE_ADD_PAGE = 3, // 设备添加页面
    DEVICE_DEL_PAGE = 4, // 设备删除页面
    DEVICE_EDIT_PAGE = 5, // 设备编辑页面
    DEVICE_GROUP_PAGE = 6, // 设备分组页面
    DEVICE_ADD_GROUP_PAGE = 7, // 设备添加分组页面
    DEVICE_DEL_GROUP_PAGE = 8, // 设备删除分组页面
    DEVICE_EDIT_GROUP_PAGE = 9, // 设备编辑分组页面
    DEVICE_DATA_PAGE = 10, // 设备数据页面
    DEVICE_DATA_HISTORY_PAGE = 11, // 设备数据历史页面
    DEVICE_ALARM_PAGE = 12, // 设备告警页面
    LOG_PAGE = 13, // 日志页面
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
};

#endif // CENTERWIDGET_H
