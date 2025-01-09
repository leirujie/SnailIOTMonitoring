#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

QT_BEGIN_NAMESPACE
namespace Ui { class LogManager; }
QT_END_NAMESPACE

class LogManager : public QObject {
    Q_OBJECT

public:
    explicit LogManager(QObject *parent = nullptr);
    ~LogManager();

    enum LogLevel {
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };

    struct LogEntry {
        QString level;
        QString time;
        QString type;
        QString content;
    };

    static LogManager& instance();
    void logMessage(LogLevel level, const QString &message);
    void setLogFile(const QString &filePath);
    void setLogLevel(LogLevel level);

private:
    QFile logFile;
    QTextStream logStream;
    QMutex mutex;
    LogLevel currentLogLevel;
    QSqlDatabase db;
    QSqlQuery query;
    QFile file;
    QString logLevelToString(LogLevel level);

signals:
    void logUpdated(QString newLog);
    void logAdded(LogEntry entry);
};

#endif // LOGMANAGER_H
