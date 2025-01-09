#ifndef USERDEVICEMANAGER_H
#define USERDEVICEMANAGER_H

#include <QWidget>

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
public slots:
    // 筛选设备
    void onFilterDevices();
private:
    Ui::UserDeviceManager *ui;
};

#endif // USERDEVICEMANAGER_H
