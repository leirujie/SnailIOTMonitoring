#ifndef ADMINMENU_H
#define ADMINMENU_H

#include <QWidget>
#include "UserDatabase.h"
#include <QStackedWidget>
#include "adduserdialog.h"
#include "adminupdateuserinfodialog.h"
#include "deviceManager/admindevicemanager.h"
#include "dataMonitor/realtimedata.h"
#include "dataMonitor/historytimedata.h"
#include "logManager/logdata.h"
#include "alertManager/alertmanager.h"

class adminUpdateUserInfoDialog;

// 定义页面索引枚举
enum AdminPageIndex {
    PAGE_USER_MANAGER = 1,  //用户管理页面
    PAGE_DEVICE_MANAGER = 2,//设备管理页面
    PAGE_DATA_MANAGER = 3,  //数据管理页面
    PAGE_LOG_MANAGER = 4,   //日志管理页面
    PAGE_WARNING_MANAGER = 5 //告警管理页面
};

namespace Ui {
class AdminMenu;
}

class AdminMenu : public QWidget
{
    Q_OBJECT

public:
    explicit AdminMenu(QWidget *parent = nullptr);
    ~AdminMenu();
    QStackedWidget* getAdminMenuStackedWidget() { return m_adminStackedWidget; } //用于获取QStackedWidget对象

signals:
    void switchPage(int targetPageIndex);  // 传递目标页面索引，用于通知外部切换页面

private slots:
    void on_btnAddUserClicked();        //处理管理员添加用户
    void on_btnDelUserClicked();        //处理管理员删除用户
    void showDataManagerDialog();
    void displayUserInfoInTable();


private:
    Ui::AdminMenu *ui;
    QStackedWidget *m_adminStackedWidget;  // 用于管理AdminMenu中的页面切换
    UserDatabase *db;
    adminUpdateUserInfoDialog * updateUserInfoDialog;//用于修改用户信息对话框
    QString m_currentAdminUsername;
    QMap<QString, QString> m_currentUserInfo;
    AdminDeviceManager *adminDeviceManager;
    RealTimeData *realtimeDataPage;
    HistoryTimeData *historyDataPage;
    logdata *logDataPage;
    AlertManager *alertmanagerpage;
};

#endif // ADMINMENU_H
