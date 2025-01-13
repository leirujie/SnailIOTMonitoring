#ifndef ALERTMANAGER_H
#define ALERTMANAGER_H

#include <QDialog>
#include <QWidget>
#include <QSqlDatabase>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QPushButton>
#include <QListView>
#include <QDateTimeEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QInputDialog>
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QDebug>
#include "logManager/logdata.h"
#include "logManager/logmanager.h"

class QPushButton;
class QListView;

namespace Ui {
class AlertManager;
}

class AlertManager : public QDialog
{
    Q_OBJECT

public:
    explicit AlertManager(QWidget *parent = nullptr);
    ~AlertManager();

private slots:
    void onCreateRuleClicked();
    void onEditRuleClicked();
    void onDeleteRuleClicked();
    void onQueryAlertClicked();
    void setupDatabase();
    void loadAlarmRules();
    void setupAlertRecordModel();
    void setupSensorDatabase();
    bool showRuleDialog(QString &deviceId, QString &description, QString &condition, QString &action, QString &deviceType);

private:
    Ui::AlertManager *ui;
    QPushButton *createRuleButton;
    QPushButton *editRuleButton;
    QPushButton *deleteRuleButton;
    QListView *rulesListView;
    QSqlDatabase sensorDb;

    QDateTimeEdit *startTimeEdit;
    QDateTimeEdit *endTimeEdit;
    QComboBox *alertTypeComboBox;
    QComboBox *deviceTypeComboBox;
    QPushButton *queryAlertButton;
    QListView *alertRecordView;

    QSqlDatabase db;
    QSqlTableModel *rulesModel;
    QSqlTableModel *alertRecordModel;
};

#endif // ALERTMANAGER_H
