#ifndef CENTERWIDGET_H
#define CENTERWIDGET_H

#include <QWidget>
#include <QStackedLayout>

namespace Ui {
class centerWidget;
}

class centerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit centerWidget(QWidget *parent = nullptr);
    ~centerWidget();

signals:
    void sendMessageByCenter(QString);
public slots:

private:
    Ui::centerWidget *ui;
    QStackedLayout *m_QStackedLayout;
};

#endif // CENTERWIDGET_H
