#include "userdevicemanager.h"
#include "ui_userdevicemanager.h"
#include <QSqlQuery>
#include <QMessageBox>

UserDeviceManager::UserDeviceManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserDeviceManager)
{
    ui->setupUi(this);
    // 初始化设备表格
    ui->deviceTable->setColumnCount(7);
    ui->deviceTable->setHorizontalHeaderLabels({"ID", "名称", "类型", "位置", "制造商", "型号", "安装日期"});
    ui->deviceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->deviceTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // 禁止编辑
    ui->deviceTable->verticalHeader()->setVisible(false);//隐藏显示行号

    // 加载过滤条件（设备类型和设备位置）
    loadFilters();
    // 加载设备数据
    loadDevices();
    // 连接筛选按钮
    connect(ui->filterButton, &QPushButton::clicked, this, &UserDeviceManager::onFilterDevices);


}

void UserDeviceManager::loadDevices(const QString &typeFilter,const QString &locationFilter)
{
    ui->deviceTable->setRowCount(0);  // 清空表格

       QSqlQuery query;
       QString sql = "SELECT * FROM devices";

       bool hasFilter = false;

       // 如果有设备类型过滤条件，添加到查询语句
       if (!typeFilter.isEmpty()) {
           sql += " WHERE type = '" + typeFilter + "'";
           hasFilter = true;
       }

       // 如果有设备位置过滤条件，添加到查询语句
       if (!locationFilter.isEmpty()) {
           if (hasFilter) {
               sql += " AND location = '" + locationFilter + "'";
           } else {
               sql += " WHERE location = '" + locationFilter + "'";
               hasFilter = true;
           }
       }

       // 执行查询
       query.exec(sql);

       // 将查询到的数据插入到表格中
       while (query.next()) {
           int row = ui->deviceTable->rowCount();
           ui->deviceTable->insertRow(row);

           // 将设备信息插入表格
           for (int col = 0; col < 7; ++col) {
               ui->deviceTable->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
           }
       }


}

void UserDeviceManager::loadFilters()
{
    QSqlQuery query;

    // 加载设备类型数据
    query.exec("SELECT name FROM groups WHERE type = '设备类型'");
    ui->typeComboBox->clear();
    ui->typeComboBox->addItem("全部"); // 空白项，表示所有设备
    while (query.next()) {
        QString name = query.value(0).toString();
        ui->typeComboBox->addItem(name);
    }

    // 加载设备位置数据
    query.exec("SELECT name FROM groups WHERE type = '设备位置'");
    ui->locationComboBox->clear();
    ui->locationComboBox->addItem("全部"); // 空白项，表示所有位置
    while (query.next()) {
        QString name = query.value(0).toString();
        ui->locationComboBox->addItem(name);
    }
}

void UserDeviceManager::onFilterDevices()
{
    QString selectedType = ui->typeComboBox->currentText();  // 获取选中的设备类型
    QString selectedLocation = ui->locationComboBox->currentText(); // 获取选中的设备位置
    // 如果选中了 "全部"，则将其替换为空字符串，以表示无筛选
       if (selectedType == "全部") {
           selectedType = "";
       }
       if (selectedLocation == "全部") {
           selectedLocation = "";
       }

       // 根据筛选条件加载设备
    loadDevices(selectedType, selectedLocation); // 根据筛选条件加载设备
}



UserDeviceManager::~UserDeviceManager()
{
    delete ui;
}
