#include "widget.h"
#include "ui_widget.h"
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QApplication>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 去除边框
    setWindowFlag(Qt::FramelessWindowHint);

    m_titleBar = new titleBar(this);
    m_statusBar = new statusBar(this);
    m_centerWidget = new centerWidget(this);

    ui->titleLayout->addWidget(m_titleBar);
    ui->statusLayout->addWidget(m_statusBar);
    ui->centerLayout->addWidget(m_centerWidget);

    loadTrayIcon();
    isPressed = false;
    resizeDirection = None;
    this->installEventFilter(this);

    this->resize(850,700);

    connect(m_centerWidget,&centerWidget::sendMessageByCenter,m_statusBar,&statusBar::onSendMessageByStatusBar);
}

bool Widget::eventFilter(QObject *watched, QEvent *event) {
    if(watched == this && event->type() == QEvent::MouseButtonPress){
        QMouseEvent *m_event = dynamic_cast<QMouseEvent *>(event);
        if(m_event->buttons() & Qt::LeftButton){
            if (m_event->pos().y() < m_titleBar->height()) {
                // 如果点击在标题栏上，则可以移动窗口
                m_point = m_event->globalPos() - this->pos();
                isPressed = true;
            } else if (m_event->pos().x() > width() - 10 && m_event->pos().y() > height() - 10) {
                // 如果点击在窗口右下角，则可以调整窗口大小
                resizeDirection = BottomRight;
                m_point = m_event->globalPos();
            }
            //            qDebug() << "left按";
        }
        return true;
    }

    if(watched == this && event->type() == QEvent::MouseButtonRelease){
        isPressed = false;
        resizeDirection = None;
        //        qDebug() << "放";
        return true;
    }

    if(watched == this && event->type() == QEvent::MouseMove){
        QMouseEvent *m_event = dynamic_cast<QMouseEvent *>(event);
        if(isPressed){
            // 移动窗口
            this->move(m_event->globalPos() - m_point);
        } else if (resizeDirection != None) {
            // 调整窗口大小
            int dx = m_event->globalX() - m_point.x();
            int dy = m_event->globalY() - m_point.y();
            QSize newSize(width() + dx, height() + dy);
            this->resize(newSize);
            m_point = m_event->globalPos();
        }
        //        qDebug() << "移动";
        return true;
    }

    return false;
}

void Widget::loadTrayIcon()
{
    // 创建托盘图标对象
    trayIcon = new QSystemTrayIcon(this);
    // 设置托盘图标
    trayIcon->setIcon(QIcon(":/image/woniu.png"));
    trayIcon->setToolTip("监控平台");

    // 创建托盘菜单
    QMenu *trayMenu = new QMenu(this);
    QAction *showAction = new QAction("打开", this);
    QAction *exitAction = new QAction("退出", this);

    // 连接动作的信号和槽
    connect(showAction, &QAction::triggered, this, &Widget::show);
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

    // 将动作添加到菜单中
    trayMenu->addAction(showAction);
    trayMenu->addAction(exitAction);

    // 将菜单添加到托盘图标中
    trayIcon->setContextMenu(trayMenu);

    // 显示托盘图标
    trayIcon->show();

    // 连接托盘图标的激活信号
    connect(trayIcon, &QSystemTrayIcon::activated, this, &Widget::onTrayIconActivated);
}

void Widget::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        if (isHidden()) {
            show();
        } else {
            hide();
        }
    }
}



Widget::~Widget()
{
    delete ui;
    delete m_titleBar;
    delete m_statusBar;
    delete m_centerWidget;
    delete trayIcon;
}
