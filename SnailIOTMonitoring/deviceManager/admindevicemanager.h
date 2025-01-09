#ifndef ADMINDEVICEMANAGER_H
#define ADMINDEVICEMANAGER_H

#include <QWidget>
#include "devicedialog.h"
#include "adddevicelocationgroupdialog.h"
#include "adddevicetypegroupdialog.h"
#include <QComboBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace Ui {
class AdminDeviceManager;
}

class AdminDeviceManager : public QWidget
{
    Q_OBJECT

public:
    explicit AdminDeviceManager(QWidget *parent = nullptr);
    ~AdminDeviceManager();
    void initializeTable();    // 初始化表格
    void initializeGroupTable();//初始化组表
    void refreshTable();       // 刷新表格数据
    void loadDevices(const QString &typeFilter, const QString &locationFilter);
    void saveDeviceToDatabase(const QList<QString> &deviceInfo); // 保存设备到数据库
    void updateDeviceInDatabase(int id, const QList<QString> &deviceInfo); // 更新设备到数据库
    void deleteDeviceFromDatabase(int id); // 从数据库删除设备
    bool isDeviceInfoValid(const QList<QString> &deviceInfo);//检查添加的设备信息
    bool containsIllegalCharacters(const QString &str);//检查非法字符

    // 添加分组到数据库
    void addGroupToDatabase(const QString &groupName, const QString &type);
    // 加载分组数据
    void loadGroups();
    // 更新分组名称
    void updateGroupInDatabase(const QString &oldGroupName, const QString &oldGroupType, const QString &newGroupName);
public slots:
    void onAddDevice();       // 添加设备
    void onEditDevice();      // 编辑设备
    void onDeleteDevice();    // 删除设备
    void onQueryDevices(); //设备位置过滤

    void onAddDeviceTypeGroup(); // 添加设备类型分组
    void onAddDeviceLocationGroup();// 添加设备位置分组
    void onDeleteGroup(); // 删除分组
    void onEditGroup();// 修改分组
private:
    Ui::AdminDeviceManager *ui;
    QPushButton *groupButton;  // 管理分组按钮
    QComboBox *groupComboBox;  // 分组选择下拉框
    QSqlDatabase db;           // 数据库连接
    QStringList currentGroups; // 当前分组列表
};

#endif // ADMINDEVICEMANAGER_H
