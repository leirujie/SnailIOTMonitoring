#ifndef ADDUSERDIALOG_H
#define ADDUSERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include "UserDatabase.h"
#include "logManager/logdata.h"
#include "logManager/logmanager.h"

namespace Ui {
class AddUserDialog;
}

class AddUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddUserDialog(QWidget  *parent = nullptr);
    ~AddUserDialog();


private slots:
    void on_addButtonClicked();

private:
    Ui::AddUserDialog *ui;
    UserDatabase *db;
};

#endif // ADDUSERDIALOG_H
