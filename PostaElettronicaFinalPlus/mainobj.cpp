#include "mainobj.h"
#include "ui_mainobj.h"
#include <QtWidgets>
#include <QStackedWidget>
#include <unistd.h>
#include <QtNetwork>
#include <QMessageBox>



#define SEND "SEND"
#define RECEIVE "RECV"
#define CONN "CONN"
#define QUIT "QUIT"
#define READ "READ"
#define UNREAD "UNRD"
#define ALL "ALL"
#define GET "GET"

// 80 char size
struct st_mail_info{
    int id;
    int fromuserid;
    int touserid;
    char status[8];// READ;UNRD;ALL;
    char title[64];
};
// 12 char size
struct st_controll{
    char operation[8];
    int userid;
    char option[4];
    int id;
};
// 1024 char size
struct st_mail{
    struct st_controll controll; // 12 char size
    struct st_mail_info info; // 80 char size
    char content[920];
};
struct st_mail_list{
    int userid;
    int sum;
    struct st_mail_info list[10];
};


mainobj::mainobj(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainobj)
{
    ui->setupUi(this);
//    mydraft=new draft_mail;
//    mydustbin=new dustbin;
//    myread=new read_mail;
//    myrecieve= new recieve_mail;
//    mywritten=new written_mail;
//    mywrite=new write_mail;
//    ui->stackedWidget->addWidget(mydraft);
//    ui->stackedWidget->addWidget(mydustbin);
//    ui->stackedWidget->addWidget(myrecieve);
//    ui->stackedWidget->addWidget(mywritten);
//    ui->stackedWidget->addWidget(mywrite);
//    ui->stackedWidget->setCurrentWidget(ui->);
//    ui->;
    QTcpSocket *socket;
    socket=new QTcpSocket;
    socket->connectToHost(QHostAddress("127.0.0.1"),8000);
}

mainobj::~mainobj()
{
    delete ui;
}

//void mainobj::on_pushButton_5_clicked()
//{
//    ui->stackedWidget->setCurrentWidget(mywritten);
//    ui->stackedWidget->setCurrentWidget(ui->page);
//}

//void mainobj::on_pushButton_clicked()
//{
//    ui->stackedWidget->setCurrentWidget(mydraft);
//}

//void mainobj::on_pushButton_2_clicked()
//{
//    ui->stackedWidget->setCurrentWidget(mywrite);
//}

//void mainobj::on_pushButton_3_clicked()
//{
//    ui->stackedWidget->setCurrentWidget(myrecieve);
//}

//void mainobj::on_pushButton_4_clicked()
//{
//    ui->stackedWidget->setCurrentWidget(mydustbin);
//}

void mainobj::on_writebutton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->writepage);
}

void mainobj::on_recievebutton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->recievepage);
}

void mainobj::on_sendbutton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->writtenpage);
}

void mainobj::on_draftbutton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->draftpage);
}


void mainobj::on_dustbinbutton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->dustbinpage);
}

void mainobj::on_starbutton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->starpage);
}

//void mainobj::on_testbutton_clicked()
//{
//    QTcpSocket *socket;
//    socket=new QTcpSocket;
//    socket->connectToHost(QHostAddress("127.0.0.1"),8000);
//    QString n=ui->testbrowser->toPlainText();
//    socket->write((char*)&n);

//    QTcpSocket *client;
//    char sip[64];
//    struct st_controll controll;
//    memset(&controll, 0, sizeof(struct st_controll));

////    QString qip = ui->textIP->toPlainText();
////    QString qport = ui->textPort->toPlainText();
//    QString qmessage;

//    strcpy(sip, "127.0.0.1");

//    client = new QTcpSocket(this);
////    client->connectToHost(QHostAddress(sip), qport.toInt());

//    strcpy(controll.operation, CONN);
//    client->write((char *)&controll, sizeof(struct st_controll));
//    connect(client, &QTcpSocket::readyRead, [=]()
//    {
//        //获取对方发送的内容
//        QByteArray array = client->readAll();
//        //追加到编辑区中
//        QMessageBox::information(this, "Server Message", array);
//        client->write(QUIT);
//    });
//}

//void mainobj::on_testbrowser_anchorClicked(const QUrl &arg1)
//{

//}

void mainobj::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->readmailpage);
}
