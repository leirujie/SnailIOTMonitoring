#ifndef ADDDEVICETYPEGROUPDIALOG_H
#define ADDDEVICETYPEGROUPDIALOG_H

#include <QObject>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class AddDeviceTypeGroupDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddDeviceTypeGroupDialog(QWidget *parent = nullptr, const QString &oldGroupName = "");
    QString getGroupName() const;
private:
    void setupUi();
    QLineEdit *m_groupNameEdit;
    QString m_oldGroupName;
};

#endif // ADDDEVICETYPEGROUPDIALOG_H
