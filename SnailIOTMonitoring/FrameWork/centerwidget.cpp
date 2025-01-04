#include "centerwidget.h"
#include "ui_centerwidget.h"

centerWidget::centerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::centerWidget)
{
    ui->setupUi(this);

    m_QStackedLayout = new QStackedLayout(this);
    //m_TCPClient = new TCPClient(this);

    //m_QStackedLayout->addWidget(m_TCPClient);

    //connect(m_TCPClient,&SerialPort::sendMessageBySerial,this,&centerWidget::sendMessageByCenter);
}

centerWidget::~centerWidget()
{
    delete ui;
}

