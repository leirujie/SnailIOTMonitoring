#ifndef USERDEVICEMANAGER_H
#define USERDEVICEMANAGER_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "logManager/logdata.h"
#include "logManager/logmanager.h"

namespace Ui {
class UserDeviceManager;
}

class UserDeviceManager : public QWidget
{
    Q_OBJECT

public:
    explicit UserDeviceManager(QWidget *parent = nullptr);
    ~UserDeviceManager();
    // 用于加载设备信息
    void loadDevices(const QString &typeFilter = "", const QString &locationFilter = "");
    void loadFilters();  // 加载过滤条件
public slots:
    // 筛选设备
    void onFilterDevices();
private:
    Ui::UserDeviceManager *ui;
    QSqlDatabase db; // 数据库连接
};

#endif // USERDEVICEMANAGER_H
