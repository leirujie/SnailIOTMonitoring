#ifndef LOGDATA_H
#define LOGDATA_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QLineEdit>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QBrush>
#include <QColor>
#include <QLabel>
#include "logmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class logdata; }
QT_END_NAMESPACE

class logdata : public QDialog
{
    Q_OBJECT

public:
    logdata(QWidget *parent = nullptr);
    ~logdata();

    struct LogEntry {
        QString level;
        QString time;
        QString type;
        QString content;
    };

    void loadLogsFromFile();

private:
    Ui::logdata *ui;
    LogEntry entry;
    QFile file;
    QDateTimeEdit *startTimeEdit;
    QDateTimeEdit *endTimeEdit;
    QComboBox *typeComboBox;
    QLineEdit *keywordsLineEdit;
};

#endif // LOGDATA_H
