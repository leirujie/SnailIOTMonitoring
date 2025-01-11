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
    logDataPage = nullptr;  // 初始化logDataPage为nullptr
    // 获取数据库单例实例
    db = UserDatabase::getInstance();
    //获取当前用户信息
    QMutexLocker locker(&g_usernameMutex); // 自动锁定和解锁
    m_currentAdminUsername  = g_currentUsername;

    adminDeviceManager = new AdminDeviceManager;

    // 获取QStackedWidget对象，用于页面切换管理
    m_adminStackedWidget = ui->widgetAdminMenu;
    // 添加页面到QStackedWidget，顺序和索引对应
    m_adminStackedWidget->addWidget(ui->pageUserManager);   //添加用户管理页面
    m_adminStackedWidget->addWidget(adminDeviceManager); //添加设备管理页面
    m_adminStackedWidget->addWidget(ui->pageDataManager);   //添加数据管理页面
    m_adminStackedWidget->addWidget(ui->pageLogManager);    //添加日志管理页面
    m_adminStackedWidget->addWidget(ui->pageWarningManager);//添加告警页面
    m_adminStackedWidget->setCurrentIndex(PAGE_DEVICE_MANAGER); // 设置默认显示管理页面


    // 连接各个按钮点击信号到对应页面的槽函数
    connect(ui->btnUserManager_, &QPushButton::clicked, this, [this](){   //进入用户管理页面
        m_adminStackedWidget->setCurrentIndex(PAGE_USER_MANAGER);
        displayUserInfoInTable();//展示用户信息表
    });
    connect(ui->btnDeviceManager_, &QPushButton::clicked, this,[this](){  //进入设备管理页面
        m_adminStackedWidget->setCurrentIndex(PAGE_DEVICE_MANAGER);
    });

    connect(ui->btnDataManager_, &QPushButton::clicked, this, [this](){   // 连接进入数据管理页面
        m_adminStackedWidget->setCurrentIndex(PAGE_DATA_MANAGER);
        showDataManagerDialog();
    });

    connect(ui->btnLogManager_, &QPushButton::clicked, this,[this](){
        if (!logDataPage) {
            logDataPage = new logdata();
            m_adminStackedWidget->addWidget(logDataPage);
        }
        m_adminStackedWidget->setCurrentWidget(logDataPage);
    });

    connect(ui->btnWaringManager_, &QPushButton::clicked, this,[this](){  //连接进入告警管理页面
        if (!alertmanagerpage) {
            alertmanagerpage = new AlertManager();
            m_adminStackedWidget->addWidget(alertmanagerpage);
        }
        m_adminStackedWidget->setCurrentWidget(alertmanagerpage);
    });

    connect(ui->btnBack_, &QPushButton::clicked, this, [this](){
        emit switchPage(ACCOUNT_PWD_LOGIN_PAGE);
    });

    connect(ui->btnAddUser_, &QPushButton::clicked, this, &AdminMenu::on_btnAddUserClicked);     //添加用户
    connect(ui->btnDelUser_, &QPushButton::clicked, this, &AdminMenu::on_btnDelUserClicked);    //删除用户
    connect(ui->btnUpdateUserInfo_, &QPushButton::clicked, this, [](){                      //连接修改用户信息
        adminUpdateUserInfoDialog updateUserInfoDialog;
        updateUserInfoDialog.exec();
    });
    connect(ui->btn_refresh,&QPushButton::clicked, this,&AdminMenu::displayUserInfoInTable);   //刷新

}

// 添加用户槽函数实现
void AdminMenu::on_btnAddUserClicked()
{
    AddUserDialog addUserDialog(this); // 使用局部变量
    int result = addUserDialog.exec();

    if (result == QDialog::Accepted) {
        // 重新查询所有用户信息，获取更新后的用户列表
        QList<QMap<QString, QString>> allUsersInfo = db->queryAllUserInfo();

        qDebug() << "All Users: " << allUsersInfo;

        // 更新用户表格
        displayUserInfoInTable();
    }
}


void AdminMenu::on_btnDelUserClicked()
{
    // 获取当前表格中被选中的行
    QModelIndexList selectedRows = ui->tableWidgetUserManager->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择要删除的用户！");
        return;
    }

    // 存储要删除的用户名列表
    QList<QString> usernamesToDelete;
    int adminsToDeleteCount = 0; // 统计要删除的管理员数量

    for (const QModelIndex& index : selectedRows) {
        int row = index.row();
        QString username = ui->tableWidgetUserManager->item(row, 0)->text();
        QString role = ui->tableWidgetUserManager->item(row, 5)->text();

        // 判断，若要删除的用户是当前管理员自己，则提示不能删除该用户
        if (username == m_currentAdminUsername) {
            QMessageBox::warning(this, "提示", QString("不能删除当前登录的管理员自己（用户名：%1），请修改用户信息来变更相关设置！").arg(username));
            continue;
        }

        // 如果用户是管理员，记录下来
        if (role.toLower() == "admin") {
            adminsToDeleteCount++;
        }

        usernamesToDelete.append(username);
    }

    if (usernamesToDelete.isEmpty()) {
        QMessageBox::warning(this, "提示", "没有可删除的有效用户，请重新选择！");
        return;
    }

    // 检查删除管理员后，系统中是否还保留至少一个管理员
    if (adminsToDeleteCount > 0) {
        int currentAdminCount = db->countAdmins();
        int adminsAfterDeletion = currentAdminCount - adminsToDeleteCount;

        if (adminsAfterDeletion < 1) {
            QMessageBox::warning(this, "警告", "不能删除所有管理员账号，系统至少需要一个管理员。");
            return;
        }
    }

    // 弹出确认对话框，确认是否删除
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认删除",
                                                              QString("确定要删除选中的 %1 个用户吗？").arg(usernamesToDelete.size()),
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        bool allDeleted = true;
        QString failedUsers;

        for (const QString& username : usernamesToDelete) {
            bool deletedSuccess = db->deleteUserInfo(username);
            if (!deletedSuccess) {
                allDeleted = false;
                failedUsers += QString("'%1', ").arg(username);
            }
        }

        if (allDeleted) {
            // 刷新表格显示删除后的用户列表
            displayUserInfoInTable();
            QMessageBox::information(this, "提示", "用户删除成功！");
        } else {
            // 移除末尾的逗号和空格
            if (!failedUsers.isEmpty()) {
                failedUsers.chop(2);
            }
            QMessageBox::warning(this, "提示", QString("部分用户删除失败：%1").arg(failedUsers));
            displayUserInfoInTable(); // 刷新表格以反映删除
        }
    }
}


// 从数据库获取用户数据并显示在表格中
void AdminMenu::displayUserInfoInTable()
{
    // 清空之前的表格内容并释放之前的QTableWidgetItem对象内存
    int rowCount = ui->tableWidgetUserManager->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < ui->tableWidgetUserManager->columnCount(); ++col) {
            QTableWidgetItem* item = ui->tableWidgetUserManager->takeItem(row, col);
            delete item;
        }
    }
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
    header->setSectionResizeMode(QHeaderView::Stretch);
    // 设置行高自适应
    ui->tableWidgetUserManager->horizontalHeader()->setStretchLastSection(true);

    // 设置表头
    QStringList headerLabels = {"用户名", "密码", "邮箱", "电话", "昵称", "角色"};
    ui->tableWidgetUserManager->setHorizontalHeaderLabels(headerLabels);

    // 遍历查询到的每个用户信息
    for (int row = 0; row < allUsersInfo.size(); ++row) {
        const QMap<QString, QString>& userInfo = allUsersInfo.at(row);

        // 每一列插入数据
        ui->tableWidgetUserManager->setItem(row, 0, new QTableWidgetItem(userInfo["username"]));
        ui->tableWidgetUserManager->setItem(row, 1, new QTableWidgetItem("******"));  //使用占位符替代真实加密后的密码显示
        ui->tableWidgetUserManager->setItem(row, 2, new QTableWidgetItem(userInfo["email"]));
        ui->tableWidgetUserManager->setItem(row, 3, new QTableWidgetItem(userInfo["phone"]));
        ui->tableWidgetUserManager->setItem(row, 4, new QTableWidgetItem(userInfo["nickname"]));
        ui->tableWidgetUserManager->setItem(row, 5, new QTableWidgetItem(userInfo["role"]));
    }
}

void AdminMenu::showDataManagerDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("选择数据类型");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QPushButton *btnRealtime = new QPushButton("查看实时数据", &dialog);
    QPushButton *btnHistorical = new QPushButton("查看历史数据", &dialog);

    layout->addWidget(btnRealtime);
    layout->addWidget(btnHistorical);

    connect(btnRealtime, &QPushButton::clicked, this, [this]() {
        // 切换到实时数据页面
        realtimeDataPage = new RealTimeData(); // 实现你的实时数据页面
        m_adminStackedWidget->addWidget(realtimeDataPage);
        m_adminStackedWidget->setCurrentWidget(realtimeDataPage);
    });

    connect(btnHistorical, &QPushButton::clicked, this, [this]() {
        // 切换到历史数据页面
        historyDataPage = new HistoryTimeData(); // 实现你的历史数据页面
        m_adminStackedWidget->addWidget(historyDataPage);
        m_adminStackedWidget->setCurrentWidget(historyDataPage);
    });

    dialog.exec();
}

AdminMenu::~AdminMenu()
{
    delete ui;
    delete logDataPage;
    delete updateUserInfoDialog;

    delete realtimeDataPage;
    delete historyDataPage;
}

