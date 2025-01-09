#ifndef ADMINUPDATEUSERINFODIALOG_H
#define ADMINUPDATEUSERINFODIALOG_H

#include <QDialog>
#include "UserDatabase.h"

namespace Ui {
class adminUpdateUserInfoDialog;
}

class adminUpdateUserInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit adminUpdateUserInfoDialog(QWidget *parent = nullptr);
    ~adminUpdateUserInfoDialog();

private slots:
    void on_queryButton_clicked();  // 查询按钮点击事件
    void on_updateButton_clicked(); // 更新按钮点击事件

private:
    Ui::adminUpdateUserInfoDialog *ui;
    UserDatabase *db;  // 数据库操作对象

    QString username() const;
    QString password() const;
    QString email() const;
    QString phone() const;
    QString role() const;
};

#endif // ADMINUPDATEUSERINFODIALOG_H
