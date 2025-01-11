#include "logmanager.h"
#include <QFile>
#include <QTextStream>
#include <QSqlQuery>
#include <QMessageBox>
#include <QRegularExpression>
#include <QDebug>
#include <QDir>

LogManager::LogManager(QObject *parent)
    : QObject(parent)
    ,currentLogLevel(INFO) {

    // 创建数据库连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("SnailIOTMonitor.db");

    if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError();
        return;
    }

    // 创建日志表
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS system_logs ("
                "log_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "timestamp TEXT NOT NULL,"
                "log_type TEXT CHECK(log_type IN ('system', 'operation', 'alarm', 'exception')),"
                "log_level TEXT CHECK(log_level IN ('INFO', 'WARNING', 'ERROR', 'CRITICAL')),"
                "content TEXT NOT NULL)");

    if (query.lastError().isValid()) {
        qDebug() << "Failed to create table:" << query.lastError();
    }
}

LogManager::~LogManager() {
    if (logFile.isOpen()){
        logFile.close();
    }
}

LogManager& LogManager::instance() {
    static LogManager instance;
    return instance;
}

void LogManager::setLogFile(const QString &filePath) {
    QMutexLocker locker(&mutex);
    if (logFile.isOpen()) {
        logFile.close();
    }

    logFile.setFileName(filePath);
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        logStream.setDevice(&logFile);
    }
}

void LogManager::setLogLevel(LogLevel level) {
    QMutexLocker locker(&mutex);
    currentLogLevel = level;
}

void LogManager::logMessage(LogLevel level, const QString &logType, const QString &message) {
    QMutexLocker locker(&mutex);
    if (level < currentLogLevel || !logFile.isOpen()) {
        return;
    }

    // 获取当前时间
    QString logTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    // 将日志等级转换为字符串
    QString logLevelStr = logLevelToString(level);
    // 格式化日志条目：时间 日志等级 日志类型 日志内容
    QString logEntry = QString("[%1] [%2] [%3] %4")
                           .arg(logTime)
                           .arg(logLevelStr)
                           .arg(logType)  // 添加日志类型
                           .arg(message);

    // 写入日志文件
    logStream << logEntry << Qt::endl;
    logStream.flush();

    // 写入数据库
    QSqlQuery query;
    query.prepare("INSERT INTO system_logs (timestamp, log_type, log_level, content) VALUES (?, ?, ?, ?)");
    query.addBindValue(logTime);
    query.addBindValue(logType);  // 使用传入的日志类型
    query.addBindValue(logLevelStr);
    query.addBindValue(message);

    if (!query.exec()) {
        qDebug() << "Failed to insert log into database:" << query.lastError();
    }

    // 发出信号给界面
    LogEntry entry = {logLevelStr, logTime, logType, message};
    emit logAdded(entry);
}


QString LogManager::logLevelToString(LogLevel level) {
    switch (level) {
        case INFO: return "INFO";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        case CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}
