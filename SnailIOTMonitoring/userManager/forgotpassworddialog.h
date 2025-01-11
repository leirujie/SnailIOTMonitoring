#ifndef FORGOTPASSWORDDIALOG_H
#define FORGOTPASSWORDDIALOG_H

#include <QDialog>
#include "UserDatabase.h"

namespace Ui {
class ForgotPasswordDialog;
}

class ForgotPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ForgotPasswordDialog(QWidget *parent = nullptr);
    ~ForgotPasswordDialog();

private slots:
    void onRecoverButtonClicked();

private:
    Ui::ForgotPasswordDialog *ui;
    UserDatabase* db;

    bool isValidEmail(const QString &email);
    bool isValidPhone(const QString &phone);
};

#endif // FORGOTPASSWORDDIALOG_H
