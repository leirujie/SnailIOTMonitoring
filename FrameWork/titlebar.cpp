#include "titlebar.h"
#include "ui_titlebar.h"

titleBar::titleBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::titleBar)
{
    ui->setupUi(this);
    //parentWidget()找到当前界面的父界面
    connect(ui->pushButtonMin,&QPushButton::clicked,this,[=](){parentWidget()->showMinimized();});

    connect(ui->pushButtonMax,&QPushButton::clicked,this,[=](){
        if (parentWidget()->isMaximized()) {
            parentWidget()->showNormal();
            return;
        }
        parentWidget()->showMaximized();
    });
    
    connect(ui->pushButtonClose,&QPushButton::clicked,this,[=](){parentWidget()->close();});
}

titleBar::~titleBar()
{
    delete ui;
}
