#include "widget.h"
#include "logManager/logdata.h"
#include <QApplication>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 初始化日志管理器
    LogManager &logManager = LogManager::instance();
    logManager.setLogFile("log.txt");

    // 加载样式表
    QFile file(":/css/style.css"); // 使用资源路径
    if (file.open(QFile::ReadOnly | QFile::Text)) {
       QString styleSheet = QLatin1String(file.readAll());
       a.setStyleSheet(styleSheet);
       file.close();
    }
    QLoggingCategory::setFilterRules(QStringLiteral("*.debug=false"));


    Widget w;
    w.show();
    return a.exec();
}
