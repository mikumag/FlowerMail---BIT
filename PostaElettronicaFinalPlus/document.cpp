#include "document.h"
#include "ui_document.h"

int From_;
int mailID_;

document::document(int From, int mailID, const char* Subject, const char* Content, QWidget *parent):
    QWidget(parent),
    ui(new Ui::document)
    {
        ui->setupUi(this);
        ui->Content->setText(Content);
        ui->Subject->setText(Subject);
        ui->From->setNum(From);
        ui->MailID->setNum(mailID);
        From_ = From;
        mailID_ = mailID;
    }

document::~document()
{
    delete ui;
}


void document::on_btn_click_clicked()
{
    QString Content =  ui->Content->text();
    QString Subject =  ui->Subject->text();
    int From = From_;
    int MailID = mailID_;

    myhome=new home;

    emit ItemGoDetail(From, MailID, Subject, Content);

    this->setFocusPolicy(Qt::NoFocus);
}

void document::on_MailID_customContextMenuRequested(const QPoint &pos)
{

}
