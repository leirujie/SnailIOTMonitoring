#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class statusBar;
}

class statusBar : public QWidget
{
    Q_OBJECT

public:
    explicit statusBar(QWidget *parent = nullptr);
    ~statusBar();
public slots:
    void onSendMessageByStatusBar(QString);
private:
    Ui::statusBar *ui;
    QTimer *m_timer;
};

#endif // STATUSBAR_H
