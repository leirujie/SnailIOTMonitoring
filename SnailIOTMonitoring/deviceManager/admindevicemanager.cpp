#include "admindevicemanager.h"
#include "ui_admindevicemanager.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QInputDialog>

AdminDeviceManager::AdminDeviceManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminDeviceManager)
{
    ui->setupUi(this);
    setWindowTitle("设备管理模块");
       //初始化组件
    ui->deviceTable->resizeColumnsToContents();//这个是自动调整tableWidget表格列宽的代码
   ui->deviceTable->setColumnCount(7); // 表格列数
   ui->deviceTable->setHorizontalHeaderLabels({"ID","名称", "类型", "位置", "制造商", "型号", "安装日期"});
   ui->deviceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
   ui->deviceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
   ui->deviceTable->verticalHeader()->setVisible(false);//隐藏显示行号


       // 初始化数据库连接
               db = QSqlDatabase::addDatabase("QSQLITE");
               db.setDatabaseName("SnailIOTMonitor.db");

               if (!db.open())
               {
                   QMessageBox::critical(this, "数据库错误", "无法连接到数据库！");
                   return;
               }
               // 初始化数据库表（如果不存在则创建）
               QSqlQuery query;
               query.exec("CREATE TABLE IF NOT EXISTS devices ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "name TEXT, "
                          "type TEXT, "
                          "location TEXT, "
                          "manufacturer TEXT, "
                          "model TEXT, "
                          "install_date TEXT)");
               //测试
               qDebug() << "SQL Query: " << query.executedQuery();  // 打印最终的 SQL 查询语句
               query.exec("CREATE TABLE IF NOT EXISTS groups ("
                                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                         "name TEXT UNIQUE, "
                                         "type TEXT );");

               // 插入设备类型和设备位置的分组信息
                          query.exec("INSERT OR IGNORE INTO groups (name, type) VALUES ('温度检测器', '设备类型');");
                          query.exec("INSERT OR IGNORE INTO groups (name, type) VALUES ('湿度检测器', '设备类型');");
                          query.exec("INSERT OR IGNORE INTO groups (name, type) VALUES ('光照检测器', '设备类型');");
                          query.exec("INSERT OR IGNORE INTO groups (name, type) VALUES ('南京', '设备位置');");
                          query.exec("INSERT OR IGNORE INTO groups (name, type) VALUES ('上海', '设备位置');");
                          query.exec("INSERT OR IGNORE INTO groups (name, type) VALUES ('北京', '设备位置');");

               // 加载分组数据
                   loadGroups();
               // 信号与槽
               connect(ui->addButton, &QPushButton::clicked, this, &AdminDeviceManager::onAddDevice);
               connect(ui->editButton, &QPushButton::clicked, this, &AdminDeviceManager::onEditDevice);
               connect(ui->deleteButton, &QPushButton::clicked, this, &AdminDeviceManager::onDeleteDevice);
               // connect(ui->filterButton, &QPushButton::clicked, this, &Widget::onFilterDevices);  // 过滤按钮
                // 连接查询按钮的槽函数
                connect(ui->queryButton, &QPushButton::clicked, this, &AdminDeviceManager::onQueryDevices);

                // 连接添加设备类型分组按钮点击信号到槽函数
                connect(ui->addDeviceTypeGroupButton, &QPushButton::clicked, this, &AdminDeviceManager::onAddDeviceTypeGroup);
                // 连接添加设备位置分组按钮点击信号到槽函数
                connect(ui->addDeviceLocationGroupButton, &QPushButton::clicked, this, &AdminDeviceManager::onAddDeviceLocationGroup);
                // 连接删除分组按钮点击信号到槽函数
                connect(ui->deleteGroupButton, &QPushButton::clicked, this, &AdminDeviceManager::onDeleteGroup);
                // 连接修改分组按钮点击信号到槽函数
                connect(ui->editGroupButton, &QPushButton::clicked, this, &AdminDeviceManager::onEditGroup);
                initializeGroupTable();  // 初始化分组表格并加载数据
               loadDevices("全部", "全部");
}


void AdminDeviceManager::initializeGroupTable()
{
    // 清空表格
    ui->groupTableWidget->clear();
    ui->groupTableWidget->setRowCount(0);  // 重置行数

    // 设置列数和列头
    ui->groupTableWidget->setColumnCount(3);  // ID, 分组名称, 类型
    ui->groupTableWidget->setHorizontalHeaderLabels({"ID", "分组名称", "分组类型"});
    // 启用列自动调整
       ui->groupTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
       // 禁用编辑
           ui->groupTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
       // 查询数据库获取分组数据
    QSqlQuery query("SELECT id, name, type FROM groups");

    int row = 0;
    while (query.next()) {
        // 为每一行插入数据
        ui->groupTableWidget->insertRow(row);

        ui->groupTableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));  // ID
        ui->groupTableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));  // 分组名称
        ui->groupTableWidget->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));  // 分组类型
        ui->groupTableWidget->verticalHeader()->setVisible(false);//隐藏显示行号

        row++;
    }
}

void AdminDeviceManager::onAddDevice()
{
    DeviceDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        QList<QString> deviceInfo = dialog.getDeviceInfo();

        // 检查设备信息是否合法
        if (isDeviceInfoValid(deviceInfo))
        {
            saveDeviceToDatabase(deviceInfo);  // 保存设备信息
            loadDevices("全部", "全部");  // 刷新设备列表
        }
        else
        {
            // 提示用户设备信息不合法
            QMessageBox::warning(this, "输入错误", "设备信息不合法，请修改后再保存！");
        }
    }
}

// 判断字符串中是否包含非法字符
bool AdminDeviceManager::containsIllegalCharacters(const QString &str)
{
    // 允许字母、数字、中文字符和连字符（-）
    QRegularExpression re("^[A-Za-z0-9\u4e00-\u9fa5-]+$");

    // 如果字符串不符合规定的字符集，说明包含非法字符
    return !str.contains(re);
}

// 校验设备信息是否合法
bool AdminDeviceManager::isDeviceInfoValid(const QList<QString> &deviceInfo)
{
    // 检查设备名称是否合法
    if (deviceInfo[0].isEmpty() || deviceInfo[0].length() > 50) {
        QMessageBox::warning(this, "设备信息错误", "设备名称不能为空且不能超过50个字符！");
        return false;
    }
    if (containsIllegalCharacters(deviceInfo[0])) {
        QMessageBox::warning(this, "设备信息错误", "设备名称包含非法字符！");
        return false;
    }

    // 获取所有设备类型分组
    QSqlQuery typeQuery;
    typeQuery.exec("SELECT name FROM groups WHERE type = '设备类型'");
    QStringList validTypes;
    while (typeQuery.next()) {
        validTypes.append(typeQuery.value(0).toString());  // 将有效的设备类型添加到 validTypes 中
    }

    // 检查设备类型是否合法
    if (deviceInfo[1].isEmpty() || !validTypes.contains(deviceInfo[1])) {
        QMessageBox::warning(this, "设备信息错误", "设备类型不能为空且必须是合法的类型！");
        return false;
    }

    // 获取所有设备位置分组
    QSqlQuery locationQuery;
    locationQuery.exec("SELECT name FROM groups WHERE type = '设备位置'");
    QStringList validLocations;
    while (locationQuery.next()) {
        validLocations.append(locationQuery.value(0).toString());  // 将有效的设备位置添加到 validLocations 中
    }

    // 检查设备位置是否合法
    if (deviceInfo[2].isEmpty() || !validLocations.contains(deviceInfo[2])) {
        QMessageBox::warning(this, "设备信息错误", "设备位置必须是合法的设备位置！");
        return false;
    }

    // 检查制造商字段（如果存在）是否合法
    if (!deviceInfo[3].isEmpty() && containsIllegalCharacters(deviceInfo[3])) {
        QMessageBox::warning(this, "设备信息错误", "制造商包含非法字符！");
        return false;
    }

    // 检查型号字段（如果存在）是否合法
    if (!deviceInfo[4].isEmpty() && containsIllegalCharacters(deviceInfo[4])) {
        QMessageBox::warning(this, "设备信息错误", "型号包含非法字符！");
        return false;
    }

    return true;
}


void AdminDeviceManager::onEditDevice()
{
    int row = ui->deviceTable->currentRow();
       if (row < 0)
       {
           QMessageBox::warning(this, "警告", "请先选择要编辑的设备！");
           return;
       }
       // 确保 ID 是从设备表格的第一列获取
       int id = ui->deviceTable->item(row, 0)->text().toInt();


          if (id == 0)
          {
              QMessageBox::warning(this, "警告", "设备 ID 获取失败！");
              return;
          }

       QList<QString> deviceInfo;

       for (int col = 1; col <= 6; ++col) {
           if (col == 2) {  // 第三列是设备类型
                      // 获取设备类型（从 QComboBox 获取选择的设备类型）
                      QComboBox *comboBox = qobject_cast<QComboBox *>(ui->deviceTable->cellWidget(row, col));
                      if (comboBox) {
                          deviceInfo.append(comboBox->currentText());  // 获取当前选择的设备类型
                      } else {
                          deviceInfo.append("");  // 如果没有 QComboBox 就添加空字符串
                      }
                  } else {
                      deviceInfo.append(ui->deviceTable->item(row, col)->text());
                  }
       }

       DeviceDialog dialog(this);
       dialog.setDeviceInfo(deviceInfo);
       if (dialog.exec() == QDialog::Accepted)
       {
           deviceInfo = dialog.getDeviceInfo();
           if (isDeviceInfoValid(deviceInfo)) {
                       updateDeviceInDatabase(id, deviceInfo);  // 更新设备信息
                       loadDevices("全部", "全部"); // 重新加载设备数据
                   }

       }
}

void AdminDeviceManager::onDeleteDevice()
{
    int row = ui->deviceTable->currentRow();
        if (row < 0)
        {
            QMessageBox::warning(this, "警告", "请先选择要删除的设备！");
            return;
        }
        int id = ui->deviceTable->item(row, 0)->text().toInt();
        if (QMessageBox::question(this, "确认", "确定要删除选中的设备吗？") == QMessageBox::Yes)
        {
            deleteDeviceFromDatabase(id);
             loadDevices("全部", "全部");
        }
}
// 查询按钮的槽函数
void AdminDeviceManager::onQueryDevices()
{
    QString typeFilter = ui->typeComboBox->currentText();  // 获取设备类型过滤条件
       QString locationFilter = ui->locationFilterComboBox->currentText();  // 获取设备位置过滤条件

       // 调用 loadDevices 函数加载符合条件的设备数据
       loadDevices(typeFilter, locationFilter);
}


void AdminDeviceManager::onAddDeviceTypeGroup()
{
    AddDeviceTypeGroupDialog dialog(this);
    // 如果对话框返回 "Accepted"（即用户点击了确定）
       if (dialog.exec() == QDialog::Accepted) {
           // 获取用户输入的分组名称
           QString groupName = dialog.getGroupName();

           // 判断用户输入的分组名称是否为空
           if (groupName.isEmpty()) {
               QMessageBox::warning(this, "输入错误", "请输入有效的设备类型分组名称！");
               return;
           }

           // 如果输入有效，调用方法将分组添加到数据库
           addGroupToDatabase(groupName, "设备类型");

           // 更新设备类型下拉框中的分组
           loadGroups();
           initializeGroupTable();  // 刷新表格

       }
}

void AdminDeviceManager::onAddDeviceLocationGroup()
{
    AddDeviceLocationGroupDialog dialog(this);
       if (dialog.exec() == QDialog::Accepted) {
           QString groupName = dialog.getGroupName();
           if (!groupName.isEmpty()) {
               addGroupToDatabase(groupName, "设备位置");
           } else {
               QMessageBox::warning(this, "输入错误", "请输入有效的设备位置分组名称！");
           }
       }
}
void AdminDeviceManager::addGroupToDatabase(const QString &groupName, const QString &type)
{
    if (!db.isOpen()) {
        QMessageBox::critical(this, "数据库错误", "数据库未成功打开！");
        return;
    }
    // 检查表结构，确保有 `type` 列，如果没有则添加
        QSqlQuery checkQuery;
        checkQuery.exec("PRAGMA table_info(groups)");  // 查询表结构

        bool hasTypeColumn = false;
        while (checkQuery.next()) {
            QString columnName = checkQuery.value("name").toString();
            if (columnName == "type") {
                hasTypeColumn = true;  // 如果已有 `type` 列，标记为 true
                break;
            }
        }

        // 如果没有 `type` 列，则添加该列
        if (!hasTypeColumn) {
            QSqlQuery alterQuery;
            alterQuery.exec("ALTER TABLE groups ADD COLUMN type TEXT");  // 添加 `type` 列
            qDebug() << "Added 'type' column to groups table";
        }

        QSqlQuery query;
        // 准备插入 SQL 语句
        query.prepare("INSERT INTO groups (name, type) VALUES (:name, :type)");

        // 打印出准备执行的 SQL 语句
        qDebug() << "Prepared Query:" << query.executedQuery();

        // 绑定参数
        query.bindValue(":name", groupName);
        query.bindValue(":type", type);

        // 调试：打印绑定的参数
        qDebug() << "Group Name: " << groupName;
        qDebug() << "Type: " << type;

        // 执行查询并检查是否成功
        if (!query.exec()) {
            // 打印出错误信息
            QMessageBox::critical(this, "数据库错误", "添加分组失败：" + query.lastError().text());
            qDebug() << "Query execution failed: " << query.lastError().text();  // 打印错误信息
        } else {
            QMessageBox::information(this, "成功", "分组添加成功！");
            loadGroups();  // 重新加载分组列表
        }
}




// 删除分组
//void AdminDeviceManager::onDeleteGroup()
//{
//    // 获取用户选中的行
//    int row = ui->groupTableWidget->currentRow();

//    // 检查是否有选中的行
//    if (row == -1) {
//        QMessageBox::warning(this, "警告", "请先选择要删除的分组！");
//        return;
//    }

//    // 获取该行的 ID、分组名称和类型
//    int groupId = ui->groupTableWidget->item(row, 0)->text().toInt();  // 获取分组 ID
//    QString groupName = ui->groupTableWidget->item(row, 1)->text();     // 获取分组名称
//    QString groupType = ui->groupTableWidget->item(row, 2)->text();     // 获取分组类型

//    // 弹出确认删除对话框
//    QMessageBox::StandardButton reply;
//    reply = QMessageBox::question(this, "确认删除", "你确定要删除这个分组吗？", QMessageBox::Yes | QMessageBox::No);

//    if (reply == QMessageBox::Yes) {
//        // 删除设备表中与分组相关的设备（根据分组类型）
//        QSqlQuery deleteDevicesQuery;
//        if (groupType == "设备类型") {
//            deleteDevicesQuery.prepare("UPDATE devices SET type = NULL WHERE type = :groupName");
//        } else if (groupType == "设备位置") {
//            deleteDevicesQuery.prepare("UPDATE devices SET location = NULL WHERE location = :groupName");
//        }

//        deleteDevicesQuery.bindValue(":groupName", groupName);

//        if (deleteDevicesQuery.exec()) {
//            qDebug() << "相关设备的分组信息已删除或更新为 NULL。";
//        } else {
//            QMessageBox::critical(this, "数据库错误", "删除设备分组信息失败：" + deleteDevicesQuery.lastError().text());
//        }

//        // 执行删除分组操作
//        QSqlQuery deleteGroupQuery;
//        deleteGroupQuery.prepare("DELETE FROM groups WHERE id = :id");
//        deleteGroupQuery.bindValue(":id", groupId);

//        if (deleteGroupQuery.exec()) {
//            // 删除成功，提示并刷新
//            QMessageBox::information(this, "删除成功", "分组已成功删除！");

//            // 重新加载分组数据到下拉框和表格
//            loadGroups();
//        } else {
//            QMessageBox::critical(this, "数据库错误", "删除分组失败：" + deleteGroupQuery.lastError().text());
//        }
//    }
//}

void AdminDeviceManager::onDeleteGroup()
{
    // 获取用户选中的行
    int row = ui->groupTableWidget->currentRow();

    // 检查是否有选中的行
    if (row == -1) {
        QMessageBox::warning(this, "警告", "请先选择要删除的分组！");
        return;
    }

    // 获取该行的 ID、分组名称和类型
    int groupId = ui->groupTableWidget->item(row, 0)->text().toInt();  // 获取分组 ID
    QString groupName = ui->groupTableWidget->item(row, 1)->text();     // 获取分组名称
    QString groupType = ui->groupTableWidget->item(row, 2)->text();     // 获取分组类型

    // 弹出确认删除对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除", "你确定要删除这个分组吗？", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // 删除设备表中与分组相关的设备（根据分组类型）
        QSqlQuery deleteDevicesQuery;
        if (groupType == "设备类型") {
            deleteDevicesQuery.prepare("UPDATE devices SET type = NULL WHERE type = :groupName");
        } else if (groupType == "设备位置") {
            deleteDevicesQuery.prepare("UPDATE devices SET location = NULL WHERE location = :groupName");
        }

        deleteDevicesQuery.bindValue(":groupName", groupName);

        if (deleteDevicesQuery.exec()) {
            qDebug() << "相关设备的分组信息已删除或更新为 NULL。";
        } else {
            QMessageBox::critical(this, "数据库错误", "删除设备分组信息失败：" + deleteDevicesQuery.lastError().text());
        }

        // 执行删除分组操作
        QSqlQuery deleteGroupQuery;
        deleteGroupQuery.prepare("DELETE FROM groups WHERE id = :id");
        deleteGroupQuery.bindValue(":id", groupId);

        if (deleteGroupQuery.exec()) {
            // 删除成功，提示并刷新
            QMessageBox::information(this, "删除成功", "分组已成功删除！");

            // 重置自增长序列
            QSqlQuery resetSeqQuery;
            resetSeqQuery.exec("DELETE FROM sqlite_sequence WHERE name='groups'");

            // 重新排列分组的 ID
            rearrangeGroupIds();

            // 重新加载分组数据
            loadGroups();
        } else {
            QMessageBox::critical(this, "数据库错误", "删除分组失败：" + deleteGroupQuery.lastError().text());
        }
    }
}


void AdminDeviceManager::rearrangeGroupIds()
{
    QSqlQuery query;
    query.exec("SELECT id FROM groups ORDER BY id ASC");  // 获取所有分组的 ID，按顺序排列

    int newId = 1;  // 从 ID 1 开始

    while (query.next()) {
        int oldId = query.value(0).toInt();

        // 更新每个分组的 ID
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE groups SET id = :newId WHERE id = :oldId");
        updateQuery.bindValue(":newId", newId);
        updateQuery.bindValue(":oldId", oldId);

        if (!updateQuery.exec()) {
            qDebug() << "Failed to update group ID:" << updateQuery.lastError().text();
        } else {
            newId++;
        }
    }

    // 重新设置自增序列，以确保下一次插入 ID 按顺序递增
    QSqlQuery resetSeqQuery;
    resetSeqQuery.exec("DELETE FROM sqlite_sequence WHERE name='groups'");
}
void AdminDeviceManager::loadGroups()
{
    QSqlQuery query;
    query.exec("SELECT id, name, type FROM groups");  // 查询包括 ID 列

    if (!query.isValid()) {
        qDebug() << "Query invalid: " << query.lastError().text();
    }

    // 清空已有的下拉框数据
    ui->typeComboBox->clear();
    ui->locationFilterComboBox->clear();

    // 添加 "全部" 选项
    ui->typeComboBox->addItem("全部");
    ui->locationFilterComboBox->addItem("全部");

    // 清空表格内容
    ui->groupTableWidget->clearContents();
    ui->groupTableWidget->setRowCount(0);

    // 遍历查询结果并填充下拉框和表格
    while (query.next()) {
        int id = query.value(0).toInt();  // 获取 ID
        QString name = query.value(1).toString();
        QString type = query.value(2).toString();

        // 添加到下拉框中
        if (type == "设备类型") {
            ui->typeComboBox->addItem(name);
        } else if (type == "设备位置") {
            ui->locationFilterComboBox->addItem(name);
        }

        // 将分组添加到表格中
        int row = ui->groupTableWidget->rowCount();
        ui->groupTableWidget->insertRow(row);
        ui->groupTableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(id)));  // 第一列显示 ID
        ui->groupTableWidget->setItem(row, 1, new QTableWidgetItem(name));               // 第二列显示分组名称
        ui->groupTableWidget->setItem(row, 2, new QTableWidgetItem(type));               // 第三列显示分组类型
    }
}
void AdminDeviceManager::onEditGroup()
{
    // 获取用户选中的行
    int row = ui->groupTableWidget->currentRow();

    // 检查是否有选中的行
    if (row == -1) {
        QMessageBox::warning(this, "警告", "请先选择要编辑的分组！");
        return;
    }

    // 获取该行的 ID、分组名称和类型
    int groupId = ui->groupTableWidget->item(row, 0)->text().toInt();  // 获取分组 ID
    QString groupName = ui->groupTableWidget->item(row, 1)->text();     // 获取分组名称
    QString groupType = ui->groupTableWidget->item(row, 2)->text();     // 获取分组类型

    // 弹出编辑对话框，允许用户修改分组名称
    bool ok;
    QString newGroupName = QInputDialog::getText(this, "编辑分组", "请输入新的分组名称：", QLineEdit::Normal, groupName, &ok);

    if (ok && !newGroupName.isEmpty()) {
        // 更新分组名称
        QSqlQuery updateGroupQuery;
        updateGroupQuery.prepare("UPDATE groups SET name = :newName WHERE id = :id");
        updateGroupQuery.bindValue(":newName", newGroupName);
        updateGroupQuery.bindValue(":id", groupId);

        if (updateGroupQuery.exec()) {
            // 更新成功，提示并刷新
            QMessageBox::information(this, "更新成功", "分组名称已成功更新！");

            // 重新加载分组数据到下拉框和表格
            loadGroups();
        } else {
            QMessageBox::critical(this, "数据库错误", "更新分组名称失败：" + updateGroupQuery.lastError().text());
        }
    } else {
        QMessageBox::warning(this, "警告", "请输入有效的分组名称！");
    }
}
// 加载设备数据，根据设备类型和位置进行过滤
void AdminDeviceManager::loadDevices(const QString &typeFilter, const QString &locationFilter)
{
    // 清空表格数据
    ui->deviceTable->setRowCount(0);

    // 从数据库加载设备数据
    QSqlQuery query;
    QString sql = "SELECT * FROM devices";

    // 如果有设备类型过滤条件，添加到查询语句
    bool hasFilter = false;
    if (!typeFilter.isEmpty() && typeFilter != "全部") {
        sql += " WHERE type = '" + typeFilter + "'";
        hasFilter = true;
    }

    // 如果有设备位置过滤条件，添加到查询语句
    if (!locationFilter.isEmpty() && locationFilter != "全部") {
        if (hasFilter) {
            sql += " AND location = '" + locationFilter + "'";
        } else {
            sql += " WHERE location = '" + locationFilter + "'";
        }
    }

    query.exec(sql);  // 执行查询

    while (query.next()) {
        int row = ui->deviceTable->rowCount();
        ui->deviceTable->insertRow(row);

        // 添加设备信息到表格
        for (int col = 0; col < 7; ++col) {
            ui->deviceTable->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
    }
}

void AdminDeviceManager::saveDeviceToDatabase(const QList<QString> &deviceInfo)
{
    // 验证必填项是否为空
       if (deviceInfo[0].isEmpty()) {
           QMessageBox::warning(this, "警告", "设备名称不能为空！");
           return;
       }
       if (deviceInfo[1].isEmpty()) {
           QMessageBox::warning(this, "警告", "设备类型不能为空！");
           return;
       }
       if (deviceInfo[2].isEmpty()) {
           QMessageBox::warning(this, "警告", "设备位置不能为空！");
           return;
       }
    QSqlQuery query;
    query.prepare("INSERT INTO devices (name, type, location, manufacturer, model, install_date) "
                  "VALUES (:name, :type, :location, :manufacturer, :model, :install_date)");
    query.bindValue(":name", deviceInfo[0]);
    query.bindValue(":type", deviceInfo[1]);
    query.bindValue(":location", deviceInfo[2]);
    query.bindValue(":manufacturer", deviceInfo[3]);
    query.bindValue(":model", deviceInfo[4]);
    query.bindValue(":install_date", deviceInfo[5]);

    if (!query.exec())
        {
            QMessageBox::critical(this, "数据库错误", query.lastError().text());
        }
}

void AdminDeviceManager::updateDeviceInDatabase(int id, const QList<QString> &deviceInfo)
{
    // 验证必填项是否为空
       if (deviceInfo[0].isEmpty()) {
           QMessageBox::warning(this, "警告", "设备名称不能为空！");
           return;
       }
       if (deviceInfo[1].isEmpty()) {
           QMessageBox::warning(this, "警告", "设备类型不能为空！");
           return;
       }
       if (deviceInfo[2].isEmpty()) {
           QMessageBox::warning(this, "警告", "设备位置不能为空！");
           return;
       }
    QSqlQuery query;
    query.prepare("UPDATE devices SET name = :name, type = :type, location = :location, "
                  "manufacturer = :manufacturer, model = :model, install_date = :install_date "
                  "WHERE id = :id");
    query.bindValue(":name", deviceInfo[0]);
    query.bindValue(":type", deviceInfo[1]);
    query.bindValue(":location", deviceInfo[2]);
    query.bindValue(":manufacturer", deviceInfo[3]);
    query.bindValue(":model", deviceInfo[4]);
    query.bindValue(":install_date", deviceInfo[5]);
    query.bindValue(":id", id);

    if (!query.exec())
        {
            QMessageBox::critical(this, "数据库错误", query.lastError().text());
        }
    else {
            qDebug() << "设备更新成功，ID: " << id;
        }
}

//void AdminDeviceManager::deleteDeviceFromDatabase(int id)
//{
//    QSqlQuery query;
//    query.prepare("DELETE FROM devices WHERE id = :id");
//    query.bindValue(":id", id);

//    if (!query.exec())
//    {
//        QMessageBox::critical(this, "数据库错误", query.lastError().text());
//    }
//}

void AdminDeviceManager::deleteDeviceFromDatabase(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM devices WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        QMessageBox::critical(this, "数据库错误", query.lastError().text());
    }
    // 清除自增长计数器，确保下次插入从1开始
        query.exec("DELETE FROM sqlite_sequence WHERE name='devices';");

        // 调用 rearrangeDeviceIds 重新排列 ID
        rearrangeDeviceIds();


}


void AdminDeviceManager::rearrangeDeviceIds()
{
    QSqlQuery query;
    query.exec("SELECT id FROM devices ORDER BY id");

    int newId = 1;
    while (query.next()) {
        int oldId = query.value(0).toInt();
        // 更新设备 ID
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE devices SET id = :newId WHERE id = :oldId");
        updateQuery.bindValue(":newId", newId++);
        updateQuery.bindValue(":oldId", oldId);
        if (!updateQuery.exec()) {
            QMessageBox::critical(this, "数据库错误", updateQuery.lastError().text());
            return;
        }
    }
}
AdminDeviceManager::~AdminDeviceManager()
{
    // 清理数据库连接
    db.close();
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    delete ui;
}
