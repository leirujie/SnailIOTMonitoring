#include "realtimedata.h"
#include "ui_realtimedata.h"

RealTimeData::RealTimeData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RealTimeData)
{
    ui->setupUi(this);

    connect(ui->loadButton, &QPushButton::clicked, this, &RealTimeData::loadData);
    connect(ui->deviceTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RealTimeData::updateDeviceIDCombo);

    connect(ui->generateChartButton, &QPushButton::clicked, this, &RealTimeData::generateChart);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/Users/Admin/Desktop/sensor_data.db");

    if (!db.open()) {
        QMessageBox::critical(this, "数据库错误", db.lastError().text());
        return;
    }

    loadComboBoxData(ui->deviceTypeCombo, ui->deviceIDCombo);
}

RealTimeData::~RealTimeData()
{
    delete ui;
    db.close();
}

void RealTimeData::loadComboBoxData(QComboBox *deviceTypeCombo, QComboBox *deviceIDCombo) {
    QSqlQuery typeQuery("SELECT DISTINCT device_type FROM data ORDER BY device_type");
    while (typeQuery.next()) {
        deviceTypeCombo->addItem(typeQuery.value(0).toString());
    }

    QSqlQuery idQuery("SELECT DISTINCT device_id FROM data ORDER BY device_id");
    while (idQuery.next()) {
        deviceIDCombo->addItem(idQuery.value(0).toString());
    }
}

void RealTimeData::updateDeviceIDCombo(int index) {
    if (index <= 0) {
        ui->deviceIDCombo->clear();
        ui->deviceIDCombo->addItem("选择设备ID");
        return;
    }

    QString selectedDeviceType = ui->deviceTypeCombo->itemText(index);
    ui->deviceIDCombo->clear();
    ui->deviceIDCombo->addItem("选择设备ID");

    QSqlQuery idQuery;
    idQuery.prepare("SELECT DISTINCT device_id FROM data WHERE device_type = :device_type ORDER BY device_id");
    idQuery.bindValue(":device_type", selectedDeviceType);

    if (!idQuery.exec()) {
        QMessageBox::critical(this, "查询错误", idQuery.lastError().text());
        return;
    }

    while (idQuery.next()) {
        ui->deviceIDCombo->addItem(idQuery.value(0).toString());
    }
}

void RealTimeData::loadData() {
    QSqlQuery query;
    if (ui->deviceTypeCombo->currentIndex() > 0 && ui->deviceIDCombo->currentIndex() > 0) {
        query.prepare("SELECT * FROM data WHERE device_type = :device_type AND device_id = :device_id");
        query.bindValue(":device_type", ui->deviceTypeCombo->currentText());
        query.bindValue(":device_id", ui->deviceIDCombo->currentText());
    } else if (ui->deviceTypeCombo->currentIndex() > 0) {
        query.prepare("SELECT * FROM data WHERE device_type = :device_type");
        query.bindValue(":device_type", ui->deviceTypeCombo->currentText());
    } else {
        query.prepare("SELECT * FROM data");
    }

    if (!query.exec()) {
        QMessageBox::critical(this, "查询错误", query.lastError().text());
        return;
    }

    ui->tableWidget->setRowCount(0);

    while (query.next()) {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        QTableWidgetItem *dataIdItem = new QTableWidgetItem(query.value("data_id").toString());
        dataIdItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(row, 0, dataIdItem);

        QTableWidgetItem *deviceIdItem = new QTableWidgetItem(query.value("device_id").toString());
        deviceIdItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(row, 1, deviceIdItem);

        QTableWidgetItem *timestampItem = new QTableWidgetItem(query.value("timestamp").toString());
        timestampItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(row, 2, timestampItem);

        QTableWidgetItem *valueItem = new QTableWidgetItem(query.value("value").toString());
        valueItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(row, 3, valueItem);

        for (int i = 0; i < ui->tableWidget->columnCount(); ++i) {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }
    }

    if (ui->tableWidget->rowCount() == 0) {
        QMessageBox::information(this, "信息", "没有数据可显示。");
    }
}

void RealTimeData::generateChart() {
    if (ui->deviceTypeCombo->currentIndex() <= 0) {
        QMessageBox::warning(this, "警告", "请选择设备类型。");
        return;
    }

    if (ui->deviceIDCombo->currentIndex() <= 0) {
        QMessageBox::warning(this, "警告", "请选择设备ID。");
        return;
    }

    QString deviceType = ui->deviceTypeCombo->currentText();
    QString deviceId = ui->deviceIDCombo->currentText();

    QSqlQuery query;
    query.prepare("SELECT timestamp, value FROM data WHERE device_type = :device_type AND device_id = :device_id ORDER BY timestamp");
    query.bindValue(":device_type", deviceType);
    query.bindValue(":device_id", deviceId);

    if (!query.exec()) {
        QMessageBox::critical(this, "查询错误", query.lastError().text());
        return;
    }

    QChart *chart = new QChart();
    chart->setTitle(deviceType);

    QSplineSeries *series = new QSplineSeries();
    series->setName(deviceId);

    qreal minValue = 0;
    qreal maxValue = 0;
    bool firstValue = true;

    while (query.next()) {
        QDateTime timestamp = QDateTime::fromString(query.value("timestamp").toString(), "yyyy-MM-dd hh:mm:ss");
        qreal value = query.value("value").toDouble();
        series->append(timestamp.toMSecsSinceEpoch(), value);

        if (firstValue) {
            minValue = value;
            maxValue = value;
            firstValue = false;
        } else {
            if (value < minValue) minValue = value;
            if (value > maxValue) maxValue = value;
        }
    }

    chart->addSeries(series);

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("时间");
    axisX->setLabelFormat("%H:%M:%S");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("数值");
    axisY->setRange(minValue, maxValue);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

//    chartView->setFixedSize(800, 500);

    chartView->show();
}
