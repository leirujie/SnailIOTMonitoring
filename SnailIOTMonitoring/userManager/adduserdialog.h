#ifndef ADDUSERDIALOG_H
#define ADDUSERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include "UserDatabase.h"

namespace Ui {
class AddUserDialog;
}

class AddUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddUserDialog(QDialog *parent = nullptr);
    ~AddUserDialog();
    // 获取输入的用户信息
    QString username() const;
    QString password() const;
    QString email() const;
    QString phone() const;
    QString role() const;

private slots:
    void on_addButton_clicked();

private:
    Ui::AddUserDialog *ui;
    UserDatabase *db;
};

#endif // ADDUSERDIALOG_H
