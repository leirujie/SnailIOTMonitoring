#include "realtimedata.h"
#include "ui_realtimedata.h"

realtimedata::realtimedata(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::realtimedata)
{
    ui->setupUi(this);
}

realtimedata::~realtimedata()
{
    delete ui;
}
