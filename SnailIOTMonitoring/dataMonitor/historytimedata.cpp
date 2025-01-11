#include "historytimedata.h"
#include "ui_historytimedata.h"

HistoryTimeData::HistoryTimeData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HistoryTimeData)
{
    ui->setupUi(this);

    ui->startTimeEdit->setDateTime(QDateTime(QDate(2025, 1, 1), QTime(0, 0, 0)));
    ui->endTimeEdit->setDateTime(QDateTime(QDate(2025, 1, 1), QTime(0, 0, 0)));

    connect(ui->loadButton, &QPushButton::clicked, this, &HistoryTimeData::loadData);
    connect(ui->deviceTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &HistoryTimeData::updateDeviceIDCombo);
    connect(ui->generateChartButton, &QPushButton::clicked, this, &HistoryTimeData::generateChart);
    connect(ui->exportCSVButton, &QPushButton::clicked, this, &HistoryTimeData::exportCSV);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/Users/Admin/Desktop/sensor_data.db");

    if (!db.open()) {
        QMessageBox::critical(this, "数据库错误", db.lastError().text());
        return;
    }

    loadComboBoxData(ui->deviceTypeCombo, ui->deviceIDCombo);
}

HistoryTimeData::~HistoryTimeData()
{
    delete ui;
    db.close();
}

void HistoryTimeData::loadComboBoxData(QComboBox *deviceTypeCombo, QComboBox *deviceIDCombo) {
    QSqlQuery typeQuery("SELECT DISTINCT device_type FROM data ORDER BY device_type");
    while (typeQuery.next()) {
        deviceTypeCombo->addItem(typeQuery.value(0).toString());
    }

    QSqlQuery idQuery("SELECT DISTINCT device_id FROM data ORDER BY device_id");
    while (idQuery.next()) {
        deviceIDCombo->addItem(idQuery.value(0).toString());
    }
}

void HistoryTimeData::updateDeviceIDCombo(int index) {
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

void HistoryTimeData::loadData() {
    QSqlQuery query;
    if (ui->deviceTypeCombo->currentIndex() > 0 && ui->deviceIDCombo->currentIndex() > 0) {
        query.prepare("SELECT * FROM data WHERE device_type = :device_type AND device_id = :device_id AND timestamp BETWEEN :start_time AND :end_time");
        query.bindValue(":device_type", ui->deviceTypeCombo->currentText());
        query.bindValue(":device_id", ui->deviceIDCombo->currentText());
        query.bindValue(":start_time", ui->startTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
        query.bindValue(":end_time", ui->endTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    } else if (ui->deviceTypeCombo->currentIndex() > 0) {
        query.prepare("SELECT * FROM data WHERE device_type = :device_type AND timestamp BETWEEN :start_time AND :end_time");
        query.bindValue(":device_type", ui->deviceTypeCombo->currentText());
        query.bindValue(":start_time", ui->startTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
        query.bindValue(":end_time", ui->endTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    } else {
        query.prepare("SELECT * FROM data WHERE timestamp BETWEEN :start_time AND :end_time");
        query.bindValue(":start_time", ui->startTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
        query.bindValue(":end_time", ui->endTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
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

void HistoryTimeData::generateChart() {
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
    query.prepare("SELECT timestamp, value FROM data WHERE device_type = :device_type AND device_id = :device_id AND timestamp BETWEEN :start_time AND :end_time ORDER BY timestamp");
    query.bindValue(":device_type", deviceType);
    query.bindValue(":device_id", deviceId);
    query.bindValue(":start_time", ui->startTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":end_time", ui->endTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"));

    if (!query.exec()) {
        QMessageBox::critical(this, "查询错误", query.lastError().text());
        return;
    }

    QChart *chart = new QChart();
    chart->setTitle(deviceType);

    QSplineSeries *series = new QSplineSeries();
    series->setName(deviceId);

    while (query.next()) {
        QDateTime timestamp = QDateTime::fromString(query.value("timestamp").toString(), "yyyy-MM-dd hh:mm:ss");
        qreal value = query.value("value").toDouble();
        series->append(timestamp.toMSecsSinceEpoch(), value);
    }

    chart->addSeries(series);

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setTitleText("时间");
    axisX->setFormat("HH:mm:ss");
    axisX->setRange(ui->startTimeEdit->dateTime(), ui->endTimeEdit->dateTime());
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("数值");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    chartView->show();
}

void HistoryTimeData::exportCSV() {
    QString fileName = QFileDialog::getSaveFileName(this, "导出CSV", "", "CSV文件 (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件进行写入。");
        return;
    }

    QTextStream out(&file);
    out << "data_id,device_id,timestamp,value\n";

    QSqlQuery query;
    query.prepare("SELECT * FROM data WHERE device_type = :device_type AND device_id = :device_id AND timestamp BETWEEN :start_time AND :end_time");
    query.bindValue(":device_type", ui->deviceTypeCombo->currentText());
    query.bindValue(":device_id", ui->deviceIDCombo->currentText());
    query.bindValue(":start_time", ui->startTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":end_time", ui->endTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"));

    if (!query.exec()) {
        QMessageBox::critical(this, "查询错误", query.lastError().text());
        file.close();
        return;
    }

    while (query.next()) {
        out << query.value("data_id").toString() << ","
            << query.value("device_id").toString() << ","
            << query.value("timestamp").toString() << ","
            << query.value("value").toString() << "\n";
    }

    file.close();
    QMessageBox::information(this, "信息", "数据已成功导出到CSV文件。");
}
