#include "userdevicemanager.h"
#include "ui_userdevicemanager.h"
#include <QSqlQuery>
#include <QMessageBox>

UserDeviceManager::UserDeviceManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserDeviceManager)
{
    // 初始化设备类型筛选框
    ui->typeComboBox->clear();
    ui->typeComboBox->addItem(""); // 空白项，表示所有设备
    ui->typeComboBox->addItem("温度检测器");
    ui->typeComboBox->addItem("湿度检测器");
    ui->typeComboBox->addItem("光照检测器");
    // 初始化设备位置筛选框
    ui->locationComboBox->clear();
    ui->locationComboBox->addItem(""); // 空白项，表示所有位置
    ui->locationComboBox->addItem("办公室");
    ui->locationComboBox->addItem("厂房");
    // 设置默认没有选中项
    ui->typeComboBox->setCurrentIndex(0);
    ui->locationComboBox->setCurrentIndex(0);

    // 初始化设备表格
    ui->deviceTable->setColumnCount(7);
    ui->deviceTable->setHorizontalHeaderLabels({"ID", "名称", "类型", "位置", "制造商", "型号", "安装日期"});
    ui->deviceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->deviceTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // 禁止编辑

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

        query.exec(sql);

        while (query.next()) {
            int row = ui->deviceTable->rowCount();
            ui->deviceTable->insertRow(row);

            // 将设备信息插入表格
            for (int col = 0; col < 7; ++col) {
                ui->deviceTable->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
            }
        }
}

void UserDeviceManager::onFilterDevices()
{
    QString selectedType = ui->typeComboBox->currentText();  // 获取选中的设备类型
    QString selectedLocation = ui->locationComboBox->currentText(); // 获取选中的设备位置
    loadDevices(selectedType, selectedLocation); // 根据筛选条件加载设备
}



UserDeviceManager::~UserDeviceManager()
{
    delete ui;
}
