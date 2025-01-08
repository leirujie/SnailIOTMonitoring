#ifndef DEVICEDIALOG_H
#define DEVICEDIALOG_H

#include <QObject>
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>

class DeviceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DeviceDialog(QWidget *parent = nullptr);
    void setDeviceInfo(const QList<QString> &deviceInfo); // 设置已有设备信息
    QList<QString> getDeviceInfo() const;   // 获取设备信息
    void onSave();
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
};

#endif // DEVICEDIALOG_H
