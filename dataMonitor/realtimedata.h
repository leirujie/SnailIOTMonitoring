#ifndef REALTIMEDATA_H
#define REALTIMEDATA_H

#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QChartView>
#include <QChart>
#include <QSplineSeries>
#include <QValueAxis>
#include <QDateTime>

QT_CHARTS_USE_NAMESPACE
QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

namespace Ui { class RealTimeData; }

class RealTimeData : public QDialog {
    Q_OBJECT

public:
    explicit RealTimeData(QWidget *parent = nullptr);
    ~RealTimeData();

    void loadData();
    void loadComboBoxData(QComboBox *deviceTypeCombo, QComboBox *deviceIDCombo);
    void updateDeviceIDCombo(int index);
    void generateChart();

private:
    Ui::RealTimeData *ui;
    QSqlDatabase db;
    QTableWidget *tableWidget;
    QComboBox *deviceTypeCombo;
    QComboBox *deviceIDCombo;
};

#endif // REALTIMEDATA_H
