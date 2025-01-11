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

    // 初始化下拉框选项
    ui->typeComboBox->addItem("所有类型");
    ui->typeComboBox->addItem("INFO");
    ui->typeComboBox->addItem("WARNING");
    ui->typeComboBox->addItem("ERROR");
    ui->typeComboBox->addItem("CRITICAL");

    // 确保控件已经正确初始化
    connect(ui->findPushButton, &QPushButton::clicked, this, &logdata::loadLogsFromFile);
}

logdata::~logdata()
{
    delete ui;
}

void logdata::loadLogsFromFile() {
    QString filePath = "log.txt"; // 日志文件路径
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
    ui->tableWidget->setRowCount(0); // 清空表格内容

    // 正则表达式匹配新的日志格式：时间 等级 类型 内容
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

        QString time = match.captured(1);   // 捕获时间
        QDateTime logDateTime = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");

        if (!logDateTime.isValid()) {
            qDebug() << "Invalid date format:" << time;
            continue;
        }

        QString level = match.captured(2); // 捕获等级
        QString type = match.captured(3);  // 捕获类型
        QString content = match.captured(4); // 捕获日志内容

        // 检查时间范围
        if (logDateTime >= fromDateTime && logDateTime <= toDateTime) {
            // 检查日志类型
            if (logType == "所有类型" || logType == level) {
                // 检查关键字
                if (keyword.isEmpty() || content.contains(keyword, Qt::CaseInsensitive)) {
                    hasLogs = true;
                    int row = ui->tableWidget->rowCount();
                    ui->tableWidget->insertRow(row);

                    // 创建 QTableWidgetItem 并设置颜色
                    QTableWidgetItem *levelItem = new QTableWidgetItem(level);
                    if (level == "INFO") {
                        levelItem->setForeground(QBrush(Qt::green)); // INFO 绿色
                    } else if (level == "WARNING") {
                        levelItem->setForeground(QBrush(Qt::yellow)); // WARNING 黄色
                    } else if (level == "ERROR") {
                        levelItem->setForeground(QBrush(Qt::red)); // ERROR 红色
                    } else if (level == "CRITICAL") {
                        levelItem->setForeground(QBrush(QColor(128, 0, 128))); // CRITICAL 紫色
                    }

                    QTableWidgetItem *timeItem = new QTableWidgetItem(time);    // 时间
                    QTableWidgetItem *typeItem = new QTableWidgetItem(type);    // 类型
                    QTableWidgetItem *contentItem = new QTableWidgetItem(content); // 内容

                    // 设置单元格内容
                    ui->tableWidget->setItem(row, 0, levelItem);   // 等级
                    ui->tableWidget->setItem(row, 1, timeItem);    // 时间
                    ui->tableWidget->setItem(row, 2, typeItem);    // 类型
                    ui->tableWidget->setItem(row, 3, contentItem); // 内容
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
        ui->tableWidget->resizeColumnsToContents(); // 自动调整列宽
        ui->tableWidget->viewport()->update(); // 刷新表格
    }
}
