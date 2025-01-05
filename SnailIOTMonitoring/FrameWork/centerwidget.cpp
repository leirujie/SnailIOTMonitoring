#include "centerwidget.h"
#include "ui_centerwidget.h"

centerWidget::centerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::centerWidget)
{
    ui->setupUi(this);
    // 创建各个页面指针指向的对象
    launchscrren = new LaunchScrren;
    loginPage = new LoginPage;
    registerPage = new RegisterPage;
    m_QStackedLayout = new QStackedLayout(this);

    //添加页面
    m_QStackedLayout->addWidget(launchscrren);//启动页面
    m_QStackedLayout->addWidget(loginPage);
    m_QStackedLayout->addWidget(registerPage);
    m_QStackedLayout->setCurrentWidget(launchscrren);

    // 页面切换
    connect(launchscrren, &LaunchScrren::switchPage, this, &centerWidget::onSwitchPage);
    connect(loginPage, &LoginPage::switchPage, this, &centerWidget::onSwitchPage);
    connect(registerPage, &RegisterPage::switchPage, this, &centerWidget::onSwitchPage);
    connect(launchscrren,&LaunchScrren::sendMsg,this,&centerWidget::sendMessageByCenter);
}

void centerWidget::onSwitchPage(int targetPageIndex)
{
    switch (targetPageIndex) {
    case LAUNCH_SCREEN_PAGE:
        m_QStackedLayout->setCurrentWidget(launchscrren);
        break;
    case ACCOUNT_PWD_LOGIN_PAGE:
        m_QStackedLayout->setCurrentWidget(loginPage);
        break;
    case REGISTER_PAGE:
        m_QStackedLayout->setCurrentWidget(registerPage);
        break;
    default:
        break;
    }
}


centerWidget::~centerWidget()
{
    delete ui;
}

