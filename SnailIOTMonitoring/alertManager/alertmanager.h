#ifndef ALERTMANAGER_H
#define ALERTMANAGER_H

#include <QDialog>
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QDateTimeEdit>
#include <QComboBox>

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
private:
    // 告警规则管理模块
    QPushButton *createRuleButton;
    QPushButton *editRuleButton;
    QPushButton *deleteRuleButton;
    QListView *rulesListView;
    QSqlDatabase sensorDb;

    // 告警记录模块
    QDateTimeEdit *startTimeEdit;
    QDateTimeEdit *endTimeEdit;
    QComboBox *alertTypeComboBox;
    QComboBox *deviceTypeComboBox;
    QPushButton *queryAlertButton;
    QListView *alertRecordView;

    // 数据库
    QSqlDatabase db;
    QSqlTableModel *rulesModel;
    QSqlTableModel *alertRecordModel;

    // 初始化
    void setupDatabase();
    void loadAlarmRules();
    void setupAlertRecordModel();
    void setupSensorDatabase();

    // 弹窗输入规则数据
    bool showRuleDialog(QString &deviceId, QString &description, QString &condition, QString &action, QString &deviceType);

private slots:
    // 告警规则模块的槽函数
    void onCreateRuleClicked();
    void onEditRuleClicked();
    void onDeleteRuleClicked();

    // 告警记录查询的槽函数
    void onQueryAlertClicked();
//    void loadAlertRecords();

private:
    Ui::AlertManager *ui;
};

#endif // ALERTMANAGER_H
