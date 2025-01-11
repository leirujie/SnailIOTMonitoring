#include "adminmenuframe.h"
#include "ui_adminmenuframe.h"

AdminMenuFrame::AdminMenuFrame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminMenuFrame)
{
    ui->setupUi(this);
}

AdminMenuFrame::~AdminMenuFrame()
{
    delete ui;
}
