#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>

namespace Ui {
class titleBar;
}

class titleBar : public QWidget
{
    Q_OBJECT

public:
    explicit titleBar(QWidget *parent = nullptr);
    ~titleBar();
signals:
//    void sendSignalQString(QString);
private:
    Ui::titleBar *ui;
};

#endif // TITLEBAR_H
