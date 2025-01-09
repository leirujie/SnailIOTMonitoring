#include "widget.h"
#include "logManager/logdata.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 初始化日志管理器
    LogManager &logManager = LogManager::instance();
    logManager.setLogFile("log.txt");

    // 写入测试日志
    logManager.logMessage(LogManager::INFO, "Application started.");
    logManager.logMessage(LogManager::WARNING, "This is a warning.");
    logManager.logMessage(LogManager::ERROR, "An error occurred.");
    logManager.logMessage(LogManager::CRITICAL, "Critical failure!");

    Widget w;
    w.show();
    return a.exec();
}
