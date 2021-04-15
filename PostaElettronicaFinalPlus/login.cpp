#include "login.h"
#include "ui_login.h"
#include "iostream"
#include "args.h"

using namespace std;

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);

}

void login::reshow(){
    this ->show();
}

login::~login()
{
    delete ui;
}

void login::on_pushButton_clicked()
{
//    myregster=new regster;
//    myregster->show();
//    this ->close();
    this ->hide();
    regster *myregster=new regster;
    myregster ->show();
    connect(myregster,SIGNAL(sendsignal()),this,SLOT(reshow()));
}


void login::on_pushButton_2_clicked()
{
    socket = new QTcpSocket;
    myhome = new home;



    struct mail mymail;

    memset(&mymail,0,sizeof(mail));
    memcpy(mymail.controll.operation,"LOGIN",8);
    socket->abort();
    socket->connectToHost(QHostAddress("127.0.0.1"),8888);

//    cout << ui->line_username->text().toInt();

    mymail.user.id = ui->line_username->text().toInt();
    //thisUserID = ui->line_username->text().toInt(); // storage ID

    memcpy(mymail.user.password,ui->line_password->text().toStdString().data(),16);

    socket->write((char*)&mymail,sizeof(mail));

    int mailNumber;
    struct mail testMail;
    char buff[sizeof(int)];
    memset(buff, 0, sizeof(int));
    socket->read(buff,sizeof (int));
    memcpy((char*)&testMail, buff, sizeof(int));

    if(true)
    {
        //QMessageBox::information(this, "Info", "Login Success.");
        QMessageBox message(QMessageBox::NoIcon, "Welcome", "Login Successful! \n You have 1 new mail(s).");
        message.exec();
//        printf("12%d",mailNumber);
//        connect(this,SIGNAL(sendname(QString)),myhome,SLOT(getStr(QString)));
//        emit sendname(ui->line_username->text());
        myhome->show();
        connect(myhome,SIGNAL(sendsignal()),this,SLOT(reshow()));
        this ->hide();
        myhome->getmyname(ui->line_username->text());
    }else{
        QMessageBox::warning(this, "Alert", "Login Failed.");
    }

    memset(&mymail,0,sizeof(mail));

    memcpy(mymail.controll.operation,"QUIT",8);
    socket->write((char*)&mymail,sizeof (mail));   //断开与服务器的连接

}
