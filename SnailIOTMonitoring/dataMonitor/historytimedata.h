#ifndef HISTORYTIMEDATA_H
#define HISTORYTIMEDATA_H

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
#include <QDateTimeAxis>
#include <QDateTimeEdit>
#include <QFileDialog>
#include <QTextStream>

QT_CHARTS_USE_NAMESPACE
QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

namespace Ui { class HistoryTimeData; }

class HistoryTimeData : public QDialog {
    Q_OBJECT

public:
    explicit HistoryTimeData(QWidget *parent = nullptr);
    ~HistoryTimeData();

    void loadData();
    void loadComboBoxData(QComboBox *deviceTypeCombo, QComboBox *deviceIDCombo);
    void updateDeviceIDCombo(int index);
    void generateChart();
    void exportCSV();

private:
    Ui::HistoryTimeData *ui;
    QSqlDatabase db;
    QTableWidget *tableWidget;
    QComboBox *deviceTypeCombo;
    QComboBox *deviceIDCombo;
};


#endif // HISTORYTIMEDATA_H
