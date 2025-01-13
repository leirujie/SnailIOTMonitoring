#include "logdata.h"
#include "ui_logdata.h"

logdata::logdata(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::logdata)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({"等级", "时间", "日志类型", "内容"});
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->typeComboBox->addItem("所有类型");
    ui->typeComboBox->addItem("INFO");
    ui->typeComboBox->addItem("WARNING");
    ui->typeComboBox->addItem("ERROR");
    ui->typeComboBox->addItem("CRITICAL");

    connect(ui->findPushButton, &QPushButton::clicked, this, &logdata::loadLogsFromFile);
}

logdata::~logdata()
{
    delete ui;
}

void logdata::loadLogsFromFile() {
    QString filePath = "log.txt";
    QFile file(filePath);

    if (!file.exists()) {
        qDebug() << "File does not exist:" << filePath;
        QLabel *label = new QLabel("日志文件不存在", this);
        label->setAlignment(Qt::AlignCenter);
        label->setGeometry(ui->tableWidget->geometry());
        ui->tableWidget->setViewport(label);
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << filePath;
        QLabel *label = new QLabel("无法打开日志文件", this);
        label->setAlignment(Qt::AlignCenter);
        label->setGeometry(ui->tableWidget->geometry());
        ui->tableWidget->setViewport(label);
        return;
    }

    QTextStream in(&file);
    ui->tableWidget->setRowCount(0);

    QRegularExpression logPattern(R"(\[(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\] \[(\w+)\] \[(\w+)\] (.+))");

    QDateTime fromDateTime = ui->startTimeEdit->dateTime();
    QDateTime toDateTime = ui->endTimeEdit->dateTime();
    QString logType = ui->typeComboBox->currentText();
    QString keyword = ui->keywordsLineEdit->text();

    bool hasLogs = false;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QRegularExpressionMatch match = logPattern.match(line);
        if (!match.hasMatch()) {
            qDebug() << "Invalid log format:" << line;
            continue;
        }

        QString time = match.captured(1);
        QDateTime logDateTime = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");

        if (!logDateTime.isValid()) {
            qDebug() << "Invalid date format:" << time;
            continue;
        }

        QString level = match.captured(2);
        QString type = match.captured(3);
        QString content = match.captured(4);

        if (logDateTime >= fromDateTime && logDateTime <= toDateTime) {
            if (logType == "所有类型" || logType == level) {
                if (keyword.isEmpty() || content.contains(keyword, Qt::CaseInsensitive)) {
                    hasLogs = true;
                    int row = ui->tableWidget->rowCount();
                    ui->tableWidget->insertRow(row);

                    QTableWidgetItem *levelItem = new QTableWidgetItem(level);
                    if (level == "INFO") {
                        levelItem->setForeground(QBrush(Qt::green));
                    } else if (level == "WARNING") {
                        levelItem->setForeground(QBrush(Qt::yellow));
                    } else if (level == "ERROR") {
                        levelItem->setForeground(QBrush(Qt::red));
                    } else if (level == "CRITICAL") {
                        levelItem->setForeground(QBrush(QColor(128, 0, 128)));
                    }

                    QTableWidgetItem *timeItem = new QTableWidgetItem(time);
                    QTableWidgetItem *typeItem = new QTableWidgetItem(type);
                    QTableWidgetItem *contentItem = new QTableWidgetItem(content);

                    ui->tableWidget->setItem(row, 0, levelItem);
                    ui->tableWidget->setItem(row, 1, timeItem);
                    ui->tableWidget->setItem(row, 2, typeItem);
                    ui->tableWidget->setItem(row, 3, contentItem);
                }
            }
        }
    }

    file.close();

    if (!hasLogs) {
        QLabel *label = new QLabel("没有找到匹配的日志", this);
        label->setAlignment(Qt::AlignCenter);
        label->setGeometry(ui->tableWidget->geometry());
        ui->tableWidget->setViewport(label);
    } else {
        ui->tableWidget->resizeColumnsToContents();
        ui->tableWidget->viewport()->update();
    }
}

void logdata::userLoadLogsFromFile() {
    QString filePath = "log.txt";
    QFile file(filePath);

    if (!file.exists()) {
        qDebug() << "File does not exist:" << filePath;
        QLabel *label = new QLabel("日志文件不存在", this);
        label->setAlignment(Qt::AlignCenter);
        label->setGeometry(ui->tableWidget->geometry());
        ui->tableWidget->setViewport(label);
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << filePath;
        QLabel *label = new QLabel("无法打开日志文件", this);
        label->setAlignment(Qt::AlignCenter);
        label->setGeometry(ui->tableWidget->geometry());
        ui->tableWidget->setViewport(label);
        return;
    }

    QTextStream in(&file);
    ui->tableWidget->setRowCount(0);

    QRegularExpression logPattern(R"(\[(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\] \[(\w+)\] \[(\w+)\] 用户(\w+)(.+))");

    QDateTime fromDateTime = ui->startTimeEdit->dateTime();
    QDateTime toDateTime = ui->endTimeEdit->dateTime();
    QString logType = ui->typeComboBox->currentText();
    QString keyword = ui->keywordsLineEdit->text();

    bool hasLogs = false;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QRegularExpressionMatch match = logPattern.match(line);
        if (!match.hasMatch()) {
            qDebug() << "Invalid log format:" << line;
            continue;
        }

        QString time = match.captured(1);
        QDateTime logDateTime = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");

        if (!logDateTime.isValid()) {
            qDebug() << "Invalid date format:" << time;
            continue;
        }

        QString level = match.captured(2);
        QString type = match.captured(3);
        QString userRole = match.captured(4); // 提取日志中的用户名
        QString content = match.captured(5); // 提取操作内容

        if (logDateTime >= fromDateTime && logDateTime <= toDateTime) {
            if (logType == "所有类型" || logType == level) {
                if (keyword.isEmpty() || content.contains(keyword, Qt::CaseInsensitive)) {
                    if (userRole != "admin") { // 排除管理员 admin 的日志
                        hasLogs = true;
                        int row = ui->tableWidget->rowCount();
                        ui->tableWidget->insertRow(row);

                        QTableWidgetItem *levelItem = new QTableWidgetItem(level);
                        if (level == "INFO") {
                            levelItem->setForeground(QBrush(Qt::green));
                        } else if (level == "WARNING") {
                            levelItem->setForeground(QBrush(Qt::yellow));
                        } else if (level == "ERROR") {
                            levelItem->setForeground(QBrush(Qt::red));
                        } else if (level == "CRITICAL") {
                            levelItem->setForeground(QBrush(QColor(128, 0, 128)));
                        }

                        QTableWidgetItem *timeItem = new QTableWidgetItem(time);
                        QTableWidgetItem *typeItem = new QTableWidgetItem(type);
                        QTableWidgetItem *contentItem = new QTableWidgetItem(content);

                        ui->tableWidget->setItem(row, 0, levelItem);
                        ui->tableWidget->setItem(row, 1, timeItem);
                        ui->tableWidget->setItem(row, 2, typeItem);
                        ui->tableWidget->setItem(row, 3, contentItem);
                    }
                }
            }
        }
    }

    file.close();

    if (!hasLogs) {
        QLabel *label = new QLabel("没有找到匹配的日志", this);
        label->setAlignment(Qt::AlignCenter);
        label->setGeometry(ui->tableWidget->geometry());
        ui->tableWidget->setViewport(label);
    } else {
        ui->tableWidget->resizeColumnsToContents();
        ui->tableWidget->viewport()->update();
    }
}
