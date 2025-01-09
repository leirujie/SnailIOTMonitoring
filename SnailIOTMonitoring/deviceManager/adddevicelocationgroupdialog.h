#ifndef ADDDEVICELOCATIONGROUPDIALOG_H
#define ADDDEVICELOCATIONGROUPDIALOG_H

#include <QObject>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class AddDeviceLocationGroupDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddDeviceLocationGroupDialog(QWidget *parent = nullptr, const QString &oldGroupName = "");
    QString getGroupName() const;
private:
    void setupUi();
    QLineEdit *m_groupNameEdit;
    QString m_oldGroupName;
};

#endif // ADDDEVICELOCATIONGROUPDIALOG_H
