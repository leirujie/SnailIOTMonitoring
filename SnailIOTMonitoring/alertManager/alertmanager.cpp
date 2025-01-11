#include "alertmanager.h"
#include <QPushButton>
#include <QListView>
#include <QDateTimeEdit>
#include <QComboBox>
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

// 构造函数
AlertManager::AlertManager(QWidget *parent)
    : QDialog(parent)
{
    // 告警规则管理模块
    createRuleButton = new QPushButton("创建规则", this);
    editRuleButton = new QPushButton("编辑规则", this);
    deleteRuleButton = new QPushButton("删除规则", this);
    rulesListView = new QListView(this);

    // 告警记录模块
    QDateTime dateTime = QDateTime::fromString("2025-01-01 00:00:00", "yyyy-MM-dd hh:mm:ss");
    startTimeEdit = new QDateTimeEdit(dateTime, this);
    endTimeEdit = new QDateTimeEdit(dateTime, this);

    alertTypeComboBox = new QComboBox(this);
    alertTypeComboBox->addItems({"全部", "温度告警", "湿度告警", "光照强度告警"});
    queryAlertButton = new QPushButton("告警查询", this);
    alertRecordView = new QListView(this);

    // 布局
    QVBoxLayout *ruleLayout = new QVBoxLayout;
    QHBoxLayout *ruleButtonLayout = new QHBoxLayout;
    ruleButtonLayout->addWidget(createRuleButton);
    ruleButtonLayout->addWidget(editRuleButton);
    ruleButtonLayout->addWidget(deleteRuleButton);
    ruleLayout->addLayout(ruleButtonLayout);
    ruleLayout->addWidget(rulesListView);

    QHBoxLayout *alertFilterLayout = new QHBoxLayout;
    alertFilterLayout->addWidget(new QLabel("开始时间："));
    alertFilterLayout->addWidget(startTimeEdit);
    alertFilterLayout->addWidget(new QLabel("结束时间："));
    alertFilterLayout->addWidget(endTimeEdit);
    alertFilterLayout->addWidget(new QLabel("告警类型："));
    alertFilterLayout->addWidget(alertTypeComboBox);
    alertFilterLayout->addWidget(queryAlertButton);

    QVBoxLayout *alertLayout = new QVBoxLayout;
    alertLayout->addLayout(alertFilterLayout);
    alertLayout->addWidget(alertRecordView);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(ruleLayout);
    mainLayout->addLayout(alertLayout);
    setLayout(mainLayout);

    // 信号与槽连接
    connect(createRuleButton, &QPushButton::clicked, this, &AlertManager::onCreateRuleClicked);
    connect(editRuleButton, &QPushButton::clicked, this, &AlertManager::onEditRuleClicked);
    connect(deleteRuleButton, &QPushButton::clicked, this, &AlertManager::onDeleteRuleClicked);
    connect(queryAlertButton, &QPushButton::clicked, this, &AlertManager::onQueryAlertClicked);

    // 数据库初始化
    setupDatabase();
    setupSensorDatabase(); // 新增：初始化传感器数据数据库
    loadAlarmRules();
    setupAlertRecordModel();
}

// 析构函数
AlertManager::~AlertManager()
{
    if (db.isOpen())
    {
        db.close();
    }
    if (sensorDb.isOpen())
    {
        sensorDb.close();
    }
}

// 初始化数据库
void AlertManager::setupDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("SnailIOTMonitor.db");

    if (!db.open())
    {
        QMessageBox::critical(this, "错误", "无法连接到数据库：" + db.lastError().text());
        return;
    }

    QSqlQuery query;
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS alarm_rules (
            rule_id INTEGER PRIMARY KEY AUTOINCREMENT,
            device_id TEXT NOT NULL,
            device_type TEXT NOT NULL,
            description TEXT,
            condition TEXT,
            action TEXT
        )
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS alert_records (
//            alarm_id INTEGER PRIMARY KEY AUTOINCREMENT,
            device_id TEXT NOT NULL,
            timestamp TEXT,
            content TEXT,
            status TEXT,
            note TEXT
        )
    )");
}

// 加载告警规则
void AlertManager::loadAlarmRules()
{
    rulesModel = new QSqlTableModel(this, db);
    rulesModel->setTable("alarm_rules");
    rulesModel->select();
    rulesListView->setModel(rulesModel);
    rulesListView->setModelColumn(3);
}

// 初始化告警记录数据模型
void AlertManager::setupAlertRecordModel()
{
    alertRecordModel = new QSqlTableModel(this, db);
    alertRecordModel->setTable("alert_records");
    alertRecordModel->select();
    alertRecordView->setModel(alertRecordModel);
}

// 弹窗：输入规则数据
bool AlertManager::showRuleDialog(QString &deviceId, QString &description, QString &condition, QString &action, QString &deviceType)
{
    QDialog dialog(this);
    dialog.setWindowTitle("规则编辑");

    // 创建输入框
    QLineEdit *deviceIdEdit = new QLineEdit(&dialog);
    QLineEdit *descriptionEdit = new QLineEdit(&dialog);
    QLineEdit *conditionEdit = new QLineEdit(&dialog);
    QLineEdit *actionEdit = new QLineEdit(&dialog);

    QComboBox *deviceTypeComboBox = new QComboBox(&dialog);
    deviceTypeComboBox->addItems({"温度检测器", "湿度检测器", "光照检测器"});
    deviceIdEdit->setText(deviceId);
    descriptionEdit->setText(description);
    conditionEdit->setText(condition);
    actionEdit->setText(action);
    deviceTypeComboBox->setCurrentText(deviceType); // 设置设备类型下拉框的默认值

    // 使用 QFormLayout 来布局输入框
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("设备类型：", deviceTypeComboBox);
    formLayout->addRow("设备 ID：", deviceIdEdit);
    formLayout->addRow("描述：", descriptionEdit);
    formLayout->addRow("条件：", conditionEdit);
    formLayout->addRow("动作：", actionEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    formLayout->addRow(buttonBox);

    dialog.setLayout(formLayout);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        deviceId = deviceIdEdit->text();
        deviceType = deviceTypeComboBox->currentText();
        description = descriptionEdit->text();
        condition = conditionEdit->text();
        action = actionEdit->text();
        return true;
    }

    return false;
}

// 创建规则
void AlertManager::onCreateRuleClicked()
{
    QString deviceId, description, condition, action, deviceType;
    if (showRuleDialog(deviceId, description, condition, action, deviceType))
    {
        QSqlQuery query;
        query.prepare("INSERT INTO alarm_rules (device_id, device_type, description, condition, action) VALUES (?, ?, ?, ?, ?)"); // 修改 SQL 语句
        query.addBindValue(deviceId);
        query.addBindValue(deviceType);
        query.addBindValue(description);
        query.addBindValue(condition);
        query.addBindValue(action);
        if (!query.exec())
        {
            QMessageBox::critical(this, "错误", "插入规则时出错：" + query.lastError().text());
        }
        loadAlarmRules();
    }
}

// 编辑规则
void AlertManager::onEditRuleClicked()
{
    QModelIndex index = rulesListView->currentIndex();
    if (!index.isValid())
    {
        QMessageBox::warning(this, "警告", "请先选择一个规则进行编辑！");
        return;
    }

    // 从数据模型中获取当前规则的各个字段的值
    QString deviceId = rulesModel->data(rulesModel->index(index.row(), 1)).toString();
    QString deviceType = rulesModel->data(rulesModel->index(index.row(), 2)).toString();
    QString description = rulesModel->data(rulesModel->index(index.row(), 3)).toString();
    QString condition = rulesModel->data(rulesModel->index(index.row(), 4)).toString();
    QString action = rulesModel->data(rulesModel->index(index.row(), 5)).toString();

    // 显示编辑规则的对话框，并初始化输入框为当前规则的值
    if (showRuleDialog(deviceId, description, condition, action, deviceType))
    {
        QSqlQuery query;
        query.prepare("UPDATE alarm_rules SET device_id=?, device_type=?, description=?, condition=?, action=? WHERE rule_id=?"); // 修改 SQL 语句
        query.addBindValue(deviceId);
        query.addBindValue(deviceType); // 绑定设备类型
        query.addBindValue(description);
        query.addBindValue(condition);
        query.addBindValue(action);
        int ruleId = rulesModel->data(rulesModel->index(index.row(), 0)).toInt();
        query.addBindValue(ruleId);
        if (!query.exec())
        {
            QMessageBox::critical(this, "错误", "更新规则时出错：" + query.lastError().text());
        }
        loadAlarmRules();
    }
}

// 删除规则
void AlertManager::onDeleteRuleClicked()
{
    QModelIndex index = rulesListView->currentIndex();
    if (!index.isValid())
        return;

    int ruleId = rulesModel->data(rulesModel->index(index.row(), 0)).toInt();
    QSqlQuery query;
    query.prepare("DELETE FROM alarm_rules WHERE rule_id=?");
    query.addBindValue(ruleId);
    if (!query.exec())
    {
        QMessageBox::critical(this, "错误", "删除规则时出错：" + query.lastError().text());
    }
    loadAlarmRules();
}

void AlertManager::setupSensorDatabase()
{
    sensorDb = QSqlDatabase::addDatabase("QSQLITE", "sensorConnection");
    sensorDb.setDatabaseName("C:/Users/Admin/Desktop/sensor_data.db");

    if (!sensorDb.open())
    {
        QMessageBox::critical(this, "错误", "无法连接到传感器数据库：" + sensorDb.lastError().text());
        return;
    }
}

void AlertManager::onQueryAlertClicked()
{
    // 获取时间范围
    QString startTime = startTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString endTime = endTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss");

    // 查询 alarm_rules 中的所有规则
    QSqlQuery alarmRulesQuery(db); // 确保 alarmRulesQuery 被声明
    alarmRulesQuery.prepare(R"(
        SELECT device_type, device_id, condition FROM alarm_rules
    )");

    if (!alarmRulesQuery.exec())
    {
        QMessageBox::critical(this, "错误", "查询告警规则时出错：" + alarmRulesQuery.lastError().text());
        return;
    }

    // 清空旧的告警信息
    QSqlQuery deleteOldAlerts(db);
    deleteOldAlerts.exec("DELETE FROM alert_records");

    // 查询传感器数据
    QSqlQuery sensorQuery(sensorDb);
    sensorQuery.prepare(R"(
        SELECT device_type, device_id, value FROM data WHERE timestamp BETWEEN ? AND ?
    )");
    sensorQuery.addBindValue(startTime);
    sensorQuery.addBindValue(endTime);

    if (!sensorQuery.exec())
    {
        QMessageBox::critical(this, "错误", "查询传感器数据时出错：" + sensorQuery.lastError().text());
        return;
    }

    // 检查传感器数据并根据规则触发报警
    QSqlQuery insertQuery(db);
    while (sensorQuery.next())
    {
        QString sensorDeviceType = sensorQuery.value("device_type").toString();
        QString sensorDeviceId = sensorQuery.value("device_id").toString();
        double sensorValue = sensorQuery.value("value").toDouble();

        // 重置 alarmRulesQuery 以重新遍历
        alarmRulesQuery.seek(0); // Reset to the beginning of the query result

        while (alarmRulesQuery.next())
        {
            QString ruleDeviceType = alarmRulesQuery.value("device_type").toString();
            QString ruleDeviceId = alarmRulesQuery.value("device_id").toString();
            QString condition = alarmRulesQuery.value("condition").toString().trimmed(); // 修剪空格

            // 输出当前传感器信息和条件
            qDebug() << "传感器设备类型:" << sensorDeviceType
                     << "设备ID:" << sensorDeviceId
                     << "当前值:" << sensorValue
                     << "条件:" << condition;

            // 确认设备类型和 ID 匹配
            if (sensorDeviceType == ruleDeviceType && sensorDeviceId == ruleDeviceId)
            {
                // 检查条件
                if (condition.startsWith(">"))
                {
                    double threshold = condition.mid(1).toDouble(); // 注意这里的索引
                    qDebug() << "阈值:" << threshold;

                    if (sensorValue > threshold)
                    {
                        qDebug() << "触发告警插入数据库"; // 调试信息
                        QString content = QString("%1-设备%2监测到值大于%3，时间：%4")
                                              .arg(sensorDeviceType)
                                              .arg(sensorDeviceId)
                                              .arg(threshold)
                                              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

                        // 插入告警记录
                        insertQuery.prepare(R"(
                            INSERT INTO alert_records (device_id, timestamp, content, status, note) VALUES (?, ?, ?, ?, ?)
                        )");
                        insertQuery.addBindValue(sensorDeviceId);
                        insertQuery.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                        insertQuery.addBindValue(content);
                        insertQuery.addBindValue("未处理");
                        insertQuery.addBindValue("");

                        if (!insertQuery.exec())
                        {
                            qDebug() << "插入告警记录时出错：" << insertQuery.lastError().text();
                        }
                    }
                }
            }
        }
    }

    // 更新告警记录视图
    QSqlQuery alertRecordQuery(db);
    alertRecordQuery.prepare(R"(
        SELECT * FROM alert_records WHERE timestamp BETWEEN ? AND ?
    )");
    alertRecordQuery.addBindValue(startTime);
    alertRecordQuery.addBindValue(endTime);

    if (!alertRecordQuery.exec())
    {
        QMessageBox::critical(this, "错误", "查询告警记录时出错：" + alertRecordQuery.lastError().text());
        return;
    }

    // 更新视图显示
    QSqlQueryModel *customAlertRecordModel = new QSqlQueryModel(this);
    customAlertRecordModel->setQuery(alertRecordQuery);
    alertRecordView->setModel(customAlertRecordModel);
}
