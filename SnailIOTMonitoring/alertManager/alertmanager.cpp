#include "alertmanager.h"

AlertManager::AlertManager(QWidget *parent)
    : QDialog(parent)
{
    createRuleButton = new QPushButton("创建规则", this);
    editRuleButton = new QPushButton("编辑规则", this);
    deleteRuleButton = new QPushButton("删除规则", this);
    rulesListView = new QListView(this);

    QDateTime dateTime = QDateTime::fromString("2025-01-01 00:00:00", "yyyy-MM-dd hh:mm:ss");
    startTimeEdit = new QDateTimeEdit(dateTime, this);
    endTimeEdit = new QDateTimeEdit(dateTime, this);

    alertTypeComboBox = new QComboBox(this);
    alertTypeComboBox->addItems({"全部", "温度检测器", "湿度检测器", "光照检测器"});
    queryAlertButton = new QPushButton("告警查询", this);
    alertRecordView = new QListView(this);

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

    connect(createRuleButton, &QPushButton::clicked, this, &AlertManager::onCreateRuleClicked);
    connect(editRuleButton, &QPushButton::clicked, this, &AlertManager::onEditRuleClicked);
    connect(deleteRuleButton, &QPushButton::clicked, this, &AlertManager::onDeleteRuleClicked);
    connect(queryAlertButton, &QPushButton::clicked, this, &AlertManager::onQueryAlertClicked);

    setupDatabase();
    setupSensorDatabase();
    loadAlarmRules();
    setupAlertRecordModel();
}

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
            alarm_id INTEGER PRIMARY KEY AUTOINCREMENT,
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
    alertRecordView->setModelColumn(3);
}

// 弹窗：输入规则数据
bool AlertManager::showRuleDialog(QString &deviceId, QString &description, QString &condition, QString &action, QString &deviceType)
{
    QDialog dialog(this);
    dialog.setWindowTitle("规则编辑");

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
    deviceTypeComboBox->setCurrentText(deviceType);

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
        query.prepare("INSERT INTO alarm_rules (device_id, device_type, description, condition, action) VALUES (?, ?, ?, ?, ?)");
        query.addBindValue(deviceId);
        query.addBindValue(deviceType);
        query.addBindValue(description);
        query.addBindValue(condition);
        query.addBindValue(action);
        if (!query.exec())
        {
            QMessageBox::critical(this, "错误", "插入规则时出错：" + query.lastError().text());
        }
        else
        {
            LogManager::instance().logMessage(LogManager::INFO, "operation",
                                              QString("创建规则: 设备ID=%1, 设备类型=%2, 描述=%3, 条件=%4, 动作=%5")
                                                  .arg(deviceId)
                                                  .arg(deviceType)
                                                  .arg(description)
                                                  .arg(condition)
                                                  .arg(action));
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

    QString deviceId = rulesModel->data(rulesModel->index(index.row(), 1)).toString();
    QString deviceType = rulesModel->data(rulesModel->index(index.row(), 2)).toString();
    QString description = rulesModel->data(rulesModel->index(index.row(), 3)).toString();
    QString condition = rulesModel->data(rulesModel->index(index.row(), 4)).toString();
    QString action = rulesModel->data(rulesModel->index(index.row(), 5)).toString();

    if (showRuleDialog(deviceId, description, condition, action, deviceType))
    {
        QSqlQuery query;
        query.prepare("UPDATE alarm_rules SET device_id=?, device_type=?, description=?, condition=?, action=? WHERE rule_id=?");
        query.addBindValue(deviceId);
        query.addBindValue(deviceType);
        query.addBindValue(description);
        query.addBindValue(condition);
        query.addBindValue(action);
        int ruleId = rulesModel->data(rulesModel->index(index.row(), 0)).toInt();
        query.addBindValue(ruleId);
        if (!query.exec())
        {
            QMessageBox::critical(this, "错误", "更新规则时出错：" + query.lastError().text());
        }
        else
        {
            LogManager::instance().logMessage(LogManager::INFO, "operation",
                                              QString("编辑规则: 规则ID=%1, 设备ID=%2, 设备类型=%3, 描述=%4, 条件=%5, 动作=%6")
                                                  .arg(ruleId)
                                                  .arg(deviceId)
                                                  .arg(deviceType)
                                                  .arg(description)
                                                  .arg(condition)
                                                  .arg(action));
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
    QString deviceId = rulesModel->data(rulesModel->index(index.row(), 1)).toString();
    QString deviceType = rulesModel->data(rulesModel->index(index.row(), 2)).toString();
    QString description = rulesModel->data(rulesModel->index(index.row(), 3)).toString();
    QString condition = rulesModel->data(rulesModel->index(index.row(), 4)).toString();
    QString action = rulesModel->data(rulesModel->index(index.row(), 5)).toString();

    QSqlQuery query;
    query.prepare("DELETE FROM alarm_rules WHERE rule_id=?");
    query.addBindValue(ruleId);
    if (!query.exec())
    {
        QMessageBox::critical(this, "错误", "删除规则时出错：" + query.lastError().text());
    }
    else
    {
        LogManager::instance().logMessage(LogManager::INFO, "operation",
                                          QString("删除规则: 规则ID=%1, 设备ID=%2, 设备类型=%3, 描述=%4, 条件=%5, 动作=%6")
                                              .arg(ruleId)
                                              .arg(deviceId)
                                              .arg(deviceType)
                                              .arg(description)
                                              .arg(condition)
                                              .arg(action));
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
    QString startTime = startTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString endTime = endTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss");

    QString alertType = alertTypeComboBox->currentText();
    qDebug() << "时间范围:" << startTime << "到" << endTime << "，告警类型:" << alertType;

    QSqlQuery alarmRulesQuery(db);
    alarmRulesQuery.prepare("SELECT device_type, device_id, condition FROM alarm_rules");

    if (!alarmRulesQuery.exec())
    {
        qDebug() << "查询告警规则失败:" << alarmRulesQuery.lastError().text();
        return;
    }

    QString sensorQueryString = R"(
        SELECT device_type, device_id, value, timestamp FROM data WHERE timestamp BETWEEN ? AND ?
    )";

    if (alertType != "全部")
    {
        sensorQueryString += " AND device_type = ?";
        qDebug() << "添加设备类型过滤条件: " << alertType;
    }

    QSqlQuery sensorQuery(sensorDb);
    sensorQuery.prepare(sensorQueryString);
    sensorQuery.addBindValue(startTime);
    sensorQuery.addBindValue(endTime);

    if (alertType != "全部")
    {
        sensorQuery.addBindValue(alertType);
    }

    if (!sensorQuery.exec())
    {
        qDebug() << "查询传感器数据失败:" << sensorQuery.lastError().text();
        return;
    }

    while (sensorQuery.next())
    {
        QString sensorDeviceType = sensorQuery.value("device_type").toString();
        QString sensorDeviceId = sensorQuery.value("device_id").toString();
        double sensorValue = sensorQuery.value("value").toDouble();
        QString sensorTimestamp = sensorQuery.value("timestamp").toString();

        qDebug() << "传感器数据:" << sensorDeviceType << sensorDeviceId << sensorValue << sensorTimestamp;

        alarmRulesQuery.seek(0);
        while (alarmRulesQuery.next())
        {
            QString ruleDeviceType = alarmRulesQuery.value("device_type").toString();
            QString ruleDeviceId = alarmRulesQuery.value("device_id").toString();
            QString condition = alarmRulesQuery.value("condition").toString().trimmed();

            qDebug() << "告警规则:" << ruleDeviceType << ruleDeviceId << condition;

            if (sensorDeviceType == ruleDeviceType && sensorDeviceId == ruleDeviceId)
            {
                if (condition.startsWith(">"))
                {
                    double threshold = condition.mid(1).toDouble();
                    qDebug() << "阈值:" << threshold;

                    if (sensorValue > threshold)
                    {
                        qDebug() << "触发告警";
                        QString content = QString("%1-设备%2监测到值大于%3，时间：%4")
                                              .arg(sensorDeviceType)
                                              .arg(sensorDeviceId)
                                              .arg(threshold)
                                              .arg(sensorTimestamp);

                        QSqlQuery checkQuery(db);
                        checkQuery.prepare(R"(
                            SELECT COUNT(*) FROM alert_records
                            WHERE device_id = ? AND timestamp = ? AND content = ?
                        )");
                        checkQuery.addBindValue(sensorDeviceId);
                        checkQuery.addBindValue(sensorTimestamp);
                        checkQuery.addBindValue(content);

                        if (checkQuery.exec() && checkQuery.next())
                        {
                            int count = checkQuery.value(0).toInt();
                            if (count > 0)
                            {
                                qDebug() << "告警记录已存在，跳过插入";
                                continue;
                            }
                        }

                        QSqlQuery insertQuery(db);
                        insertQuery.prepare(R"(
                            INSERT INTO alert_records (device_id, timestamp, content, status, note) VALUES (?, ?, ?, ?, ?)
                        )");
                        insertQuery.addBindValue(sensorDeviceId);
                        insertQuery.addBindValue(sensorTimestamp);
                        insertQuery.addBindValue(content);
                        insertQuery.addBindValue("未处理");
                        insertQuery.addBindValue("");

                        if (!insertQuery.exec())
                        {
                            qDebug() << "插入告警记录失败:" << insertQuery.lastError().text();
                        }
                        else
                        {
                            LogManager::instance().logMessage(LogManager::WARNING, "alarm",
                                                             QString("告警触发: 设备类型=%1, 设备ID=%2, 值=%3, 阈值=%4, 时间=%5")
                                                                 .arg(sensorDeviceType)
                                                                 .arg(sensorDeviceId)
                                                                 .arg(sensorValue)
                                                                 .arg(threshold)
                                                                 .arg(sensorTimestamp));
                        }
                    }
                }
            }
        }
    }

    QSqlQuery alertRecordQuery(db);
    QString queryString = R"(
        SELECT * FROM alert_records WHERE timestamp BETWEEN ? AND ?
    )";

    if (alertType != "全部")
    {
        queryString += " AND content LIKE ?";
    }

    alertRecordQuery.prepare(queryString);
    alertRecordQuery.addBindValue(startTime);
    alertRecordQuery.addBindValue(endTime);

    if (alertType != "全部")
    {
        alertRecordQuery.addBindValue("%" + alertType + "%");
    }

    if (!alertRecordQuery.exec())
    {
        qDebug() << "查询告警记录失败:" << alertRecordQuery.lastError().text();
        return;
    }

    int recordCount = 0;
    while (alertRecordQuery.next())
    {
        recordCount++;
        qDebug() << "告警记录:" << alertRecordQuery.value("device_id").toString() << alertRecordQuery.value("timestamp").toString() << alertRecordQuery.value("content").toString();
    }
    qDebug() << "查询到的告警记录数量:" << recordCount;

    QSqlQueryModel *customAlertRecordModel = new QSqlQueryModel(this);
    customAlertRecordModel->setQuery(alertRecordQuery);
    alertRecordView->setModel(customAlertRecordModel);
}
