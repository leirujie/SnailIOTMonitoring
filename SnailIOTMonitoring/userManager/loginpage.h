#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>
#include <QString>
#include <QMessageBox>
#include <FrameWork/centerwidget.h>
#include "UserDatabase.h"
#include <QFile>
#include "usermenu.h"
#include "adminmenu.h"
#include "global.h"

namespace Ui {
class LoginPage;
}

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage();

signals:
    void switchPage(int targetPageIndex);  // 传递目标页面索引

public slots:
    void loginClicked();
    void clearInputLogin();  // 清空输入框

private:
    Ui::LoginPage *ui;
    UserDatabase *db;
    int errorCount;

};

#endif // LOGINPAGE_H
