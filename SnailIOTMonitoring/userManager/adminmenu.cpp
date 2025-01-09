#include "adminmenu.h"
#include "ui_adminmenu.h"
#include "FrameWork/centerwidget.h"
#include <QMessageBox>
#include "adduserdialog.h"
#include <QDebug>
#include <QCryptographicHash>

// 构造函数实现
AdminMenu::AdminMenu(QWidget *parent) : QWidget(parent)
{
    ui = new Ui::AdminMenu;
    ui->setupUi(this);
    // 获取数据库单例实例
    db = UserDatabase::getInstance(parent);
    //获取当前用户信息
    QMutexLocker locker(&g_usernameMutex); // 自动锁定和解锁
    m_currentAdminUsername  = g_currentUsername;

    // 获取QStackedWidget对象，用于页面切换管理
    m_adminStackedWidget = ui->widgetAdminMenu;
    // 添加页面到QStackedWidget，顺序和索引对应
    m_adminStackedWidget->addWidget(ui->pageUserManager);   //添加用户管理页面
    m_adminStackedWidget->addWidget(ui->pageDeviceManager); //添加设备管理页面
    m_adminStackedWidget->addWidget(ui->pageDataManager);   //添加数据管理页面
    m_adminStackedWidget->addWidget(ui->pageLogManager);    //添加日志管理页面
    m_adminStackedWidget->addWidget(ui->pageWarningManager);//添加告警页面

    // 连接各个按钮点击信号到对应页面的槽函数

    connect(ui->btnUserManager_, &QPushButton::clicked, this, [this](){   //进入用户管理页面
        m_adminStackedWidget->setCurrentIndex(PAGE_USER_MANAGER);
        displayUserInfoInTable();//展示用户信息表
    });
    connect(ui->btnDeviceManager_, &QPushButton::clicked, this,[this](){  //进入设备管理页面
        m_adminStackedWidget->setCurrentIndex(PAGE_DEVICE_MANAGER);
    });
    connect(ui->btnDataManager_, &QPushButton::clicked, this, [this](){   //连接进入数据管理页面
        m_adminStackedWidget->setCurrentIndex(PAGE_DATA_MANAGER);
    });
    connect(ui->btnLogManager_, &QPushButton::clicked, this,[this](){     //连接进入日志管理页面
        m_adminStackedWidget->setCurrentIndex(PAGE_LOG_MANAGER);
    });
    connect(ui->btnWaringManager_, &QPushButton::clicked, this,[this](){  //连接进入告警管理页面
        m_adminStackedWidget->setCurrentIndex(PAGE_WARNING_MANAGER);
    });
    connect(ui->btnBack_, &QPushButton::clicked, this, [this](){
        emit switchPage(ACCOUNT_PWD_LOGIN_PAGE);
    });

    connect(ui->btnAddUser_, &QPushButton::clicked, this, &AdminMenu::on_btnAddUserClicked);     //添加用户
    connect(ui->btnDelUser_, &QPushButton::clicked, this, &AdminMenu::on_btnDelUserClicked);    //删除用户
    connect(ui->btnUpdateUserInfo_, &QPushButton::clicked, this, [this](){                      //连接修改用户信息
        updateUserInfoDialog = new adminUpdateUserInfoDialog;
        updateUserInfoDialog->exec();
    });
    connect(ui->btn_refresh,&QPushButton::clicked, this,&AdminMenu::displayUserInfoInTable);   //刷新

}

// 添加用户槽函数实现
void AdminMenu::on_btnAddUserClicked()
{
    // 创建并显示添加用户的对话框
    addUserDialog = new AddUserDialog;
    int result = addUserDialog->exec();

    // 如果用户在对话框中点击了确认
    if (result == QDialog::Accepted) {
        // 重新查询所有用户信息，获取更新后的用户列表
        QList<QMap<QString, QString>> allUsersInfo = db->queryAllUserInfo();

        qDebug() << "All Users: " << allUsersInfo;

        // 获取表格当前的行数
        int currentRowCount = ui->tableWidgetUserManager->rowCount();

        // 在表格末尾插入新行
        ui->tableWidgetUserManager->insertRow(currentRowCount);

        // 获取最后一行的用户信息（这里密码应该是加密后的形式展示，假设 AddUserDialog 类能返回加密后的密码）
        const QMap<QString, QString>& newUserInfo = allUsersInfo.last();
        QString encryptedPassword = newUserInfo["password"];

        // 将新用户信息填入表格对应列，密码使用加密后的形式
        ui->tableWidgetUserManager->setItem(currentRowCount, 0, new QTableWidgetItem(newUserInfo["username"]));
        ui->tableWidgetUserManager->setItem(currentRowCount, 1, new QTableWidgetItem(encryptedPassword));
        ui->tableWidgetUserManager->setItem(currentRowCount, 2, new QTableWidgetItem(newUserInfo["email"]));
        ui->tableWidgetUserManager->setItem(currentRowCount, 3, new QTableWidgetItem(newUserInfo["phone"]));
        ui->tableWidgetUserManager->setItem(currentRowCount, 4, new QTableWidgetItem(newUserInfo["nickname"]));
        ui->tableWidgetUserManager->setItem(currentRowCount, 5, new QTableWidgetItem(newUserInfo["role"]));
    }
}

//删除用户槽函数实现
void AdminMenu::on_btnDelUserClicked()
{
    // 获取当前表格中被选中的行
    QModelIndexList selectedRows = ui->tableWidgetUserManager->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择要删除的用户！");
        return;
    }
    int selectedRow = selectedRows.first().row();
    QString username = ui->tableWidgetUserManager->item(selectedRow, 0)->text();

    // 弹出确认对话框，确认是否删除
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认删除",
                                                              QString("确定要删除用户 '%1' 吗？").arg(username),
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        db->deleteUserInfo(username);
        // 刷新表格显示删除后的用户列表
        displayUserInfoInTable();
        QMessageBox::information(this, "提示", "用户删除成功！");
    }
}

// 从数据库获取用户数据并显示在表格中
void AdminMenu::displayUserInfoInTable()
{
    // 清空之前的表格内容
    ui->tableWidgetUserManager->clearContents();
    // 移除表格中所有行
    ui->tableWidgetUserManager->setRowCount(0);

    // 查询所有用户信息
    QList<QMap<QString, QString>> allUsersInfo = db->queryAllUserInfo();

    // 设置表格的行数和列数
    ui->tableWidgetUserManager->setRowCount(allUsersInfo.size());
    ui->tableWidgetUserManager->setColumnCount(6);

    // 设置列宽自适应
    QHeaderView* header = ui->tableWidgetUserManager->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch); // 让列宽自适应窗口宽度
    // 设置行高自适应
    ui->tableWidgetUserManager->horizontalHeader()->setStretchLastSection(true);

    // 设置表头
    ui->tableWidgetUserManager->setHorizontalHeaderLabels({"用户名", "密码", "邮箱", "电话", "昵称", "角色"});

    // 遍历查询到的每个用户信息
    for (int row = 0; row < allUsersInfo.size(); ++row) {
        const QMap<QString, QString>& userInfo = allUsersInfo.at(row);

        // 每一列插入数据
        ui->tableWidgetUserManager->setItem(row, 0, new QTableWidgetItem(userInfo["username"]));
        ui->tableWidgetUserManager->setItem(row, 1, new QTableWidgetItem(userInfo["password"]));
        ui->tableWidgetUserManager->setItem(row, 2, new QTableWidgetItem(userInfo["email"]));
        ui->tableWidgetUserManager->setItem(row, 3, new QTableWidgetItem(userInfo["phone"]));
        ui->tableWidgetUserManager->setItem(row, 4, new QTableWidgetItem(userInfo["nickname"]));
        ui->tableWidgetUserManager->setItem(row, 5, new QTableWidgetItem(userInfo["role"]));
    }
}


AdminMenu::~AdminMenu()
{
    delete ui;
}

