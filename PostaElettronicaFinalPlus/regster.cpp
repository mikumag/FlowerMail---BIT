#include "regster.h"
#include "ui_regster.h"
#include "unistd.h"

regster::regster(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::regster)
{
    ui->setupUi(this);

}

regster::~regster()
{
    delete ui;
}

void regster::on_pushButton_clicked()
{
    socket = new QTcpSocket;

    struct mail mymail;

    memset(&mymail,0,sizeof(mail));
    memcpy(mymail.controll.operation,"REGIST",8);
    socket->abort();
    socket->connectToHost(QHostAddress("127.0.0.1"),8888);  //发送socket操作请求：注册用户
    QString ID=ui->line_mail->text().toStdString().data();

    mymail.user.id=ID.toInt();
    memcpy(mymail.user.password,ui->line_password->text().toStdString().data(),16);
    socket->write((char*)&mymail,sizeof (mail));    //发送注册信息

    memset(&mymail,0,sizeof(mail));
    socket->write((char*)&mymail,sizeof (mail));   //断开与服务器的连接
//    socket->
    emit sendsignal();          //发送信号，返回登录界面
    this->close();
}

void regster::on_line_lastName_cursorPositionChanged(int arg1, int arg2)
{

}
