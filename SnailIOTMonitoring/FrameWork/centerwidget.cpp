#include "centerwidget.h"
#include "ui_centerwidget.h"
#include <QDebug>

centerWidget::centerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::centerWidget)
{
    ui->setupUi(this);
    // 创建各个页面指针指向的对象
    launchscrren = new LaunchScrren;
    loginPage = new LoginPage;
    registerPage = new RegisterPage;
    adminMenu = new AdminMenu;
    userMenu = new UserMenu;
    m_QStackedLayout = new QStackedLayout(this);

    //添加页面
    m_QStackedLayout->addWidget(launchscrren);
    m_QStackedLayout->addWidget(loginPage);
    m_QStackedLayout->addWidget(registerPage);
    m_QStackedLayout->addWidget(userMenu);
    m_QStackedLayout->addWidget(adminMenu);

    m_QStackedLayout->setCurrentWidget(launchscrren);

    // 页面切换
    connect(launchscrren, &LaunchScrren::switchPage, this, &centerWidget::onSwitchPage);//启动页面
    connect(loginPage, &LoginPage::switchPage, this, &centerWidget::onSwitchPage);//登陆页面
    connect(registerPage, &RegisterPage::switchPage, this, &centerWidget::onSwitchPage);//注册页面
    connect(userMenu, &UserMenu::switchPage, this, &centerWidget::onSwitchPage);//用户菜单
    connect(adminMenu, &AdminMenu::switchPage, this, &centerWidget::onSwitchPage);//管理员菜单

    connect(launchscrren,&LaunchScrren::sendMsg,this,&centerWidget::sendMessageByCenter);
}

void centerWidget::onSwitchPage(int targetPageIndex)
{
    switch (targetPageIndex) {
    case LAUNCH_SCREEN_PAGE:
        m_QStackedLayout->setCurrentWidget(launchscrren);  // 启动页面
        break;
    case ACCOUNT_PWD_LOGIN_PAGE:
        m_QStackedLayout->setCurrentWidget(loginPage);  // 登录页面
        break;
    case REGISTER_PAGE:
        m_QStackedLayout->setCurrentWidget(registerPage);  // 注册页面
        break;
    case Menu_User_PAGE:
        m_QStackedLayout->setCurrentWidget(userMenu);  // 用户菜单
        break;
    case Menu_ADMIN_PAGE:
        m_QStackedLayout->setCurrentWidget(adminMenu);  // 管理员菜单
        break;
    default:
        break;
    }

    // 在管理员菜单中，根据按钮的点击切换具体页面
    if (targetPageIndex == Menu_ADMIN_PAGE) {
        //获取 AdminMenu 的 QStackedWidget 对象
        QStackedWidget *adminMenuStackedWidget = adminMenu->getAdminMenuStackedWidget();
        if (adminMenuStackedWidget) {
            adminMenuStackedWidget->setCurrentIndex(targetPageIndex - Menu_ADMIN_PAGE);
        }
    }
    // 在用户菜单中，根据按钮的点击切换具体页面
    if (targetPageIndex == Menu_User_PAGE) {
        // 获取UserMenu 的 QStackedWidget 对象
        QStackedWidget *userMenuStackedWidget = userMenu->getUserMenuStackedWidget();
        if (userMenuStackedWidget) {
            userMenuStackedWidget->setCurrentIndex(targetPageIndex - Menu_User_PAGE);
        }
    }
    if (targetPageIndex == ACCOUNT_PWD_LOGIN_PAGE) {
        loginPage->clearInputLogin();
    }
    if (targetPageIndex == REGISTER_PAGE) {
        registerPage->clearInputRegister();
    }

}



centerWidget::~centerWidget()
{
    delete ui;
}

