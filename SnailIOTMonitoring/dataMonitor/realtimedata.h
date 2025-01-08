#ifndef REALTIMEDATA_H
#define REALTIMEDATA_H

#include <QDialog>

namespace Ui {
class realtimedata;
}

class realtimedata : public QDialog
{
    Q_OBJECT

public:
    explicit realtimedata(QWidget *parent = nullptr);
    ~realtimedata();

private:
    Ui::realtimedata *ui;
};

#endif // REALTIMEDATA_H
