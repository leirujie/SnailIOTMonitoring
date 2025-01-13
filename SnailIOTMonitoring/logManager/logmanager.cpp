#include "logmanager.h"

LogManager::LogManager(QObject *parent)
    : QObject(parent)
    ,currentLogLevel(INFO) {

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("SnailIOTMonitor.db");

    if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError();
        return;
    }

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

    QString logTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString logLevelStr = logLevelToString(level);
    QString logEntry = QString("[%1] [%2] [%3] %4")
                           .arg(logTime)
                           .arg(logLevelStr)
                           .arg(logType)
                           .arg(message);

    logStream << logEntry << Qt::endl;
    logStream.flush();

    QSqlQuery query;
    query.prepare("INSERT INTO system_logs (timestamp, log_type, log_level, content) VALUES (?, ?, ?, ?)");
    query.addBindValue(logTime);
    query.addBindValue(logType);
    query.addBindValue(logLevelStr);
    query.addBindValue(message);

    if (!query.exec()) {
        qDebug() << "Failed to insert log into database:" << query.lastError();
    }

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
