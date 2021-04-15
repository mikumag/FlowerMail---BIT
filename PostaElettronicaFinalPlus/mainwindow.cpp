#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    a = new mainobj;
    a -> show();
}

void MainWindow::on_pushButton_2_clicked()
{
    mylogin=new login;
    mylogin->show();
}

void MainWindow::on_pushButton_3_clicked()
{
    ho=new home;
    ho->show();
}
