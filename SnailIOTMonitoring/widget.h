#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "FrameWork/titlebar.h"
#include "FrameWork/statusbar.h"
#include "FrameWork/centerwidget.h"
#include <QSystemTrayIcon>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    enum ResizeDirection {
        None,
        BottomRight
    };

    void loadTrayIcon();
    bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::Widget *ui;
    titleBar *m_titleBar;
    statusBar *m_statusBar;
    centerWidget *m_centerWidget;
    QSystemTrayIcon *trayIcon;
    ResizeDirection resizeDirection;

    QPoint m_point;
    bool isPressed;
    bool isFullScreen;
};

#endif // WIDGET_H
