#ifndef LAUNCHSCRREN_H
#define LAUNCHSCRREN_H

#include <QWidget>
#include "FrameWork/centerwidget.h"
namespace Ui {
class LaunchScrren;
}

class LaunchScrren : public QWidget
{
    Q_OBJECT

public:
    explicit LaunchScrren(QWidget *parent = nullptr);
    ~LaunchScrren();

signals:
    void switchPage(int targetPageIndex);  // 传递目标页面索引
    void sendMsg(const QString);

private:
    Ui::LaunchScrren *ui;
};

#endif // LAUNCHSCRREN_H
