#ifndef ADMINMENUFRAME_H
#define ADMINMENUFRAME_H

#include <QWidget>

namespace Ui {
class AdminMenuFrame;
}

class AdminMenuFrame : public QWidget
{
    Q_OBJECT

public:
    explicit AdminMenuFrame(QWidget *parent = nullptr);
    ~AdminMenuFrame();

private:
    Ui::AdminMenuFrame *ui;
};

#endif // ADMINMENUFRAME_H
