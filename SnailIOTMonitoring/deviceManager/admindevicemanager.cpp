#include "admindevicemanager.h"
#include "ui_admindevicemanager.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

AdminDeviceManager::AdminDeviceManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminDeviceManager)
{
    ui->setupUi(this);
    setWindowTitle("设备管理模块");
       //初始化组件
       ui->deviceTable->setColumnCount(7); // 表格列数
       ui->deviceTable->setHorizontalHeaderLabels({"ID","名称", "类型", "位置", "制造商", "型号", "安装日期"});
       ui->deviceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
       ui->deviceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

       // 初始化设备类型的 ComboBox
       ui->typeComboBox->addItem("全部");  // 添加一个空项
       ui->typeComboBox->addItem("温度检测器");
       ui->typeComboBox->addItem("湿度检测器");
       ui->typeComboBox->addItem("光照检测器");
       // 设置默认没有选中项
       ui->typeComboBox->setCurrentIndex(0);  // 默认选择"请选择"项
       ui->locationFilterComboBox->addItem("全部");  // 默认选择“全部”
       ui->locationFilterComboBox->addItem("办公室");
       ui->locationFilterComboBox->addItem("厂房");
       // 初始化数据库连接
               db = QSqlDatabase::addDatabase("QSQLITE");
               db.setDatabaseName("SnailIOTMonitoring.db");

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

               query.exec("CREATE TABLE IF NOT EXISTS groups ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "name TEXT UNIQUE)");


               // 信号与槽
               connect(ui->addButton, &QPushButton::clicked, this, &AdminDeviceManager::onAddDevice);
               connect(ui->editButton, &QPushButton::clicked, this, &AdminDeviceManager::onEditDevice);
               connect(ui->deleteButton, &QPushButton::clicked, this, &AdminDeviceManager::onDeleteDevice);
               // connect(ui->filterButton, &QPushButton::clicked, this, &Widget::onFilterDevices);  // 过滤按钮
                // 连接查询按钮的槽函数
                connect(ui->queryButton, &QPushButton::clicked, this, &AdminDeviceManager::onQueryDevices);



               loadDevices("全部", "全部");
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

bool AdminDeviceManager::containsIllegalCharacters(const QString &str)
{
    // 允许字母、数字、中文字符
       QRegularExpression re("[A-Za-z0-9\u4e00-\u9fa5]+");

       // 如果字符串不符合规定的字符集，说明包含非法字符
       return !str.contains(re);
}
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

    // 检查设备类型是否合法
    QString validTypes[] = {"温度检测器", "湿度检测器", "光照检测器"};
    if (deviceInfo[1].isEmpty() || !std::any_of(std::begin(validTypes), std::end(validTypes),
                                                 [&deviceInfo](const QString& type){ return type == deviceInfo[1]; })) {
        QMessageBox::warning(this, "设备信息错误", "设备类型不能为空且必须是合法的类型！");
        return false;
    }

    // 检查设备位置是否合法
        QString validLocations[] = {"办公室", "厂房"};
        if (deviceInfo[2].isEmpty() || !std::any_of(std::begin(validLocations), std::end(validLocations),
                                                     [&deviceInfo](const QString& location){ return location == deviceInfo[2]; })) {
            QMessageBox::warning(this, "设备信息错误", "设备位置必须选择“办公室”或“厂房”！");
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
           updateDeviceInDatabase(id, deviceInfo);// 更新设备信息
           loadDevices("全部", "全部");// 重新加载设备数据
           //refreshTable();
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

void AdminDeviceManager::initializeTable()
{
   // ui->deviceTable->setRowCount(0);

    ui->deviceTable->setColumnCount(7); // 设置表格列数为 7，包括 ID 列
        ui->deviceTable->setHorizontalHeaderLabels({"ID", "名称", "类型", "位置", "制造商", "型号", "安装日期"}); // 设置列标题

}


// 查询按钮的槽函数
void AdminDeviceManager::onQueryDevices()
{
    QString typeFilter = ui->typeComboBox->currentText();  // 获取设备类型过滤条件
       QString locationFilter = ui->locationFilterComboBox->currentText();  // 获取设备位置过滤条件

       // 调用 loadDevices 函数加载符合条件的设备数据
       loadDevices(typeFilter, locationFilter);
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

void AdminDeviceManager::deleteDeviceFromDatabase(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM devices WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        QMessageBox::critical(this, "数据库错误", query.lastError().text());
    }
}

AdminDeviceManager::~AdminDeviceManager()
{
    // 清理数据库连接
    db.close();
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    delete ui;
}
