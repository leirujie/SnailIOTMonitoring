#include "statusbar.h"
#include "ui_statusbar.h"

statusBar::statusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::statusBar)
{
    ui->setupUi(this);

    m_timer = new QTimer(this);

    connect(m_timer,&QTimer::timeout,this,[=](){ui->labelStatus->clear();});
}

statusBar::~statusBar()
{
    delete ui;
    delete m_timer;
}

void statusBar::onSendMessageByStatusBar(QString msg)
{
    ui->labelStatus->setText(msg);
    m_timer->start(2000);
}
