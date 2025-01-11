#ifndef DEVICEDIALOG_H
#define DEVICEDIALOG_H

#include <QObject>
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include "logManager/logdata.h"
#include "logManager/logmanager.h"

// 前向声明 Widget 类
class AdminDeviceManager;

class DeviceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DeviceDialog(QWidget *parent = nullptr);
    void setDeviceInfo(const QList<QString> &deviceInfo); // 设置已有设备信息
    QList<QString> getDeviceInfo() const;   // 获取设备信息
    void onSave();
    void loadGroupData();
private:
    QLineEdit *nameEdit;
    QLineEdit *typeEdit;
    QLineEdit *locationEdit;
    QLineEdit *manufacturerEdit;
    QLineEdit *modelEdit;
    QDateEdit *installDateEdit;

    QPushButton *saveButton;
    QPushButton *cancelButton;
    QComboBox *typeComboBox;
    QComboBox *locationComboBox;
    int id;
    AdminDeviceManager *parentWidget;// 父窗口指针
};

#endif // DEVICEDIALOG_H
