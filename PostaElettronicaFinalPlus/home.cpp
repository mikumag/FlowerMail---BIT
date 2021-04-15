#include "home.h"
#include "ui_home.h"
#include "document.h"
#include "QStackedWidget"
#include <QtWidgets>
#include <QString>
#include <QFont>
#include <unistd.h>
#include <QtNetwork>
#include <QMessageBox>
#include "iostream"
#include "args.h"

using namespace std;


struct mail mymail;
QString myname;
int mail_id;
int client_id;

home::home(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::home)
{
    ui->setupUi(this);
    socket = new  QTcpSocket;


//    ui->pages_widget->setCurrentWidget(ui->pageInbox);

}

void home::getmyname(QString str)
{
    myname = str;
}

void home::readmail(int From, int mailID, QString Subject, QString Content)
{
    ui->pages_widget->setCurrentWidget(ui->pageRead);
    ui->readContent->setText(Content);
    ui->readSubject->setText(Subject);
    ui->From->setNum(From);
    mail_id = mailID;
}

void home::readbin(int From, int mailID, QString Subject, QString Content)
{
    ui->pages_widget->setCurrentWidget(ui->pageRead);
    ui->readContent->setText(Content);
    ui->readSubject->setText(Subject);
    ui->From->setNum(From);
    mail_id = mailID;
}

home::~home()
{
    delete ui;
}

void home::getname(QString str)
{
     myname=str;  //自定义linEdit对象，将oldStr 传入dialog并显示在linEdit中。
}



void home::on_btn_compose_clicked()
{
    ui->pages_widget->setCurrentWidget(ui->pageCompose);

}

void home::on_btn_inbox_clicked()
{
    ui->pages_widget->setCurrentWidget(ui->pageInbox);
}

void home::on_btn_starred_clicked()
{
    ui->pages_widget->setCurrentWidget(ui->pageStarred);
}

void home::on_btn_sent_clicked()
{
    ui->pages_widget->setCurrentWidget(ui->pageSent);
}

void home::on_btn_drafts_clicked()
{
    ui->pages_widget->setCurrentWidget(ui->pageDraft);
}

void home::on_btn_bin_clicked()
{
    ui->pages_widget->setCurrentWidget(ui->pageBin);
}

void home::on_btn_send_clicked()
{
    memset(&mymail,0,sizeof(mail));
    memcpy(mymail.controll.operation,"SEND",8);
    socket->abort();
    socket->connectToHost(QHostAddress("127.0.0.1"),8888);
    QString Recipients=ui->lineRecipients->text();
    QString ccRecipients=ui->lineCc->text();
    QString bccRecipients=ui->lineRecip->text();
    QString title=ui->lineSubject->text();

    QString content = ui->TextEdit->toPlainText();
    memcpy(mymail.content,content.toStdString().data(),880);
    int i=1;
    while(i--)
    {
        QString sender=Recipients;
        mymail.info.receiverID = sender.toInt();
        mymail.info.senderID = myname.toInt();

        memcpy(mymail.info.title,title.toStdString().data(),64);
        socket->write((char*)&mymail,sizeof(mail));
    }

    memset(&mymail,0,sizeof(mail));

    memcpy(mymail.controll.operation,"QUIT",8);
    socket->write((char*)&mymail,sizeof (mail));   //断开与服务器的连接
}

void home::on_btn_refresh_clicked()
{
    home *mynewhome= new home;
    ui=mynewhome->ui;
    memset(&mymail,0,sizeof(mail));
    memcpy(mymail.controll.operation,"R_ALL",8);

    socket->abort();
    socket->connectToHost(QHostAddress("127.0.0.1"),8888);
    mymail.user.id = myname.toInt();    // get hostID from storage
    socket->write((char*)&mymail,sizeof (mail));    // write socket for operation

    ui->listWidgetInbox->clear();

    // receive from server
    connect(socket, &QTcpSocket::readyRead, [=]() {

        char buff[51208];
        memset(buff, 0, 51208);
        socket->read(buff, 51208);

        cout << "rec";
        struct mailList myMailList;
        memset(&myMailList, 0 ,sizeof(mailList));
        memcpy(&myMailList, buff, sizeof(struct mailList));

        ui->listWidgetInbox->clear();

        for(int i=0; i<45; i++) {
            if(myMailList.list[i].info.mailID > 0 && myMailList.list[i].info.mailID < 300) {

                QListWidgetItem *aItem = new QListWidgetItem(ui->listWidgetInbox);
                aItem->setSizeHint(QSize(0,35));
                aItem->setFlags(0);
                ui->listWidgetInbox->addItem(aItem);

                document *mydou;
                mydou=new document(myMailList.list[i].info.senderID, myMailList.list[i].info.mailID, myMailList.list[i].info.title, myMailList.list[i].content);
           //     ui->listWidgetInbox->setItemWidget(aItem,new document(myMailList.list[i].info.senderID, myMailList.list[i].info.title, myMailList.list[i].content));
                ui->listWidgetInbox->setItemWidget(aItem,mydou);

                QString Content =  myMailList.list[i].content;
                QString Subject =  myMailList.list[i].info.title;
                int From = myMailList.list[i].info.senderID;
                int MailID = myMailList.list[i].info.mailID;

                connect(mydou,SIGNAL( ItemGoDetail(int, int, QString, QString)),this,SLOT(readmail(int, int, QString, QString)));
            }
        }
        memset(&mymail,0,sizeof(mail));
        memcpy(mymail.controll.operation,"QUIT",8);
        socket->write((char*)&mymail,sizeof (mail));   //断开与服务器的连接

    });
    this->close();
    mynewhome->getmyname(myname);
    mynewhome->ui->pages_widget->setCurrentWidget(mynewhome->ui->pageInbox);
    mynewhome->show();
}


void home::on_btn_help_clicked()
{
    QMessageBox message(QMessageBox::NoIcon, "Help", "Welcome to neusoft mail! For more instructions please contact 123@neusoft.");
    message.exec();
}

void home::on_listWidgetInbox_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{

}

void home::on_btn_logout_clicked()
{
    emit sendsignal();
    this->close();
}

void home::on_btn_refresh_4_clicked()
{
    home *mynewhome= new home;
    ui=mynewhome->ui;

    memset(&mymail,0,sizeof(mail));
    memcpy(mymail.controll.operation,"R_DRAFT",8);

    socket->abort();
    socket->connectToHost(QHostAddress("127.0.0.1"),8888);
    mymail.user.id = myname.toInt();    // get hostID from storage
    socket->write((char*)&mymail,sizeof (mail));    // write socket for operation

    ui->listWidgetDraft->clear();

    // receive from server
    connect(socket, &QTcpSocket::readyRead, [=]() {

        char buff[51208];
        memset(buff, 0, 51208);
        socket->read(buff, 51208);

        cout << "bin";

        struct mailList myMailList;
        memset(&myMailList, 0 ,sizeof(mailList));
        memcpy(&myMailList, buff, sizeof(struct mailList));

        ui->listWidgetDraft->clear();

        for(int i=0; i<45; i++) {
            if(myMailList.list[i].info.mailID > 0 && myMailList.list[i].info.mailID < 300) {
                QListWidgetItem *aItem = new QListWidgetItem(ui->listWidgetDraft);
                aItem->setSizeHint(QSize(0,40));
                aItem->setFlags(0);
                ui->listWidgetDraft->addItem(aItem);

                document *mydou;
                mydou=new document(myMailList.list[i].info.senderID, myMailList.list[i].info.mailID, myMailList.list[i].info.title, myMailList.list[i].content);

                ui->listWidgetDraft->setItemWidget(aItem,mydou);

                int From = myMailList.list[i].info.senderID;
                int MailID = myMailList.list[i].info.mailID;
                QString Subject =  myMailList.list[i].info.title;
                QString Content =  myMailList.list[i].content;

                connect(mydou,SIGNAL( ItemGoDetail(int, int, QString, QString)),this,SLOT(readbin(int, int, QString, QString)));
            }
        }
        memset(&mymail,0,sizeof(mail));
        memcpy(mymail.controll.operation,"QUIT",8);
        socket->write((char*)&mymail,sizeof (mail));   //断开与服务器的连接


    });
    this->close();
    mynewhome->getmyname(myname);
    mynewhome->ui->pages_widget->setCurrentWidget(mynewhome->ui->pageDraft);
    mynewhome->show();
}

void home::on_fontComboBox_activated(const QString &arg1)
{
    QFont font;
    font.setFamily(arg1);
    ui->TextEdit->setFont(font);
}

void home::on_comboBox_2_activated(int index)
{
    ui->TextEdit->setFontPointSize(index);
    ui->TextEdit->setFocus();
}

void home::on_btn_refresh_5_clicked()
{
    home *mynewhome= new home;
    ui=mynewhome->ui;

    memset(&mymail,0,sizeof(mail));
    memcpy(mymail.controll.operation,"R_BIN",8);

    socket->abort();
    socket->connectToHost(QHostAddress("127.0.0.1"),8888);
    mymail.user.id = myname.toInt();    // get hostID from storage
    socket->write((char*)&mymail,sizeof (mail));    // write socket for operation

    ui->listWidgetBin->clear();

    // receive from server
    connect(socket, &QTcpSocket::readyRead, [=]() {

        char buff[51208];
        memset(buff, 0, 51208);
        socket->read(buff, 51208);

        cout << "bin";

        struct mailList myMailList;
        memset(&myMailList, 0 ,sizeof(mailList));
        memcpy(&myMailList, buff, sizeof(struct mailList));

        ui->listWidgetBin->clear();

        for(int i=0; i<45; i++) {
            if(myMailList.list[i].info.mailID > 0 && myMailList.list[i].info.mailID < 300) {
                QListWidgetItem *aItem = new QListWidgetItem(ui->listWidgetBin);
                aItem->setSizeHint(QSize(0,40));
                aItem->setFlags(0);
                ui->listWidgetBin->addItem(aItem);

                document *mydou;
                mydou=new document(myMailList.list[i].info.senderID, myMailList.list[i].info.mailID, myMailList.list[i].info.title, myMailList.list[i].content);

                ui->listWidgetBin->setItemWidget(aItem,mydou);

                int From = myMailList.list[i].info.senderID;
                int MailID = myMailList.list[i].info.mailID;
                QString Subject =  myMailList.list[i].info.title;
                QString Content =  myMailList.list[i].content;

                connect(mydou,SIGNAL( ItemGoDetail(int, int, QString, QString)),this,SLOT(readbin(int, int, QString, QString)));
            }
        }
        memset(&mymail,0,sizeof(mail));
        memcpy(mymail.controll.operation,"QUIT",8);
        socket->write((char*)&mymail,sizeof (mail));   //断开与服务器的连接


    });
    this->close();
    mynewhome->getmyname(myname);
    mynewhome->ui->pages_widget->setCurrentWidget(mynewhome->ui->pageBin);
    mynewhome->show();
}

void home::on_btn_refresh_2_clicked()
{
    home *mynewhome= new home;
    ui=mynewhome->ui;

    memset(&mymail,0,sizeof(mail));
    memcpy(mymail.controll.operation,"R_STAR",8);

    socket->abort();
    socket->connectToHost(QHostAddress("127.0.0.1"),8888);
    mymail.user.id = myname.toInt();    // get hostID from storage
    socket->write((char*)&mymail,sizeof (mail));    // write socket for operation

    ui->listWidgetStarred->clear();

    // receive from server
    connect(socket, &QTcpSocket::readyRead, [=]() {

        char buff[51208];
        memset(buff, 0, 51208);
        socket->read(buff, 51208);

        cout << "bin";

        struct mailList myMailList;
        memset(&myMailList, 0 ,sizeof(mailList));
        memcpy(&myMailList, buff, sizeof(struct mailList));

        ui->listWidgetStarred->clear();

        for(int i=0; i<45; i++) {
            if(myMailList.list[i].info.mailID > 0 && myMailList.list[i].info.mailID < 300) {
                QListWidgetItem *aItem = new QListWidgetItem(ui->listWidgetStarred);
                aItem->setSizeHint(QSize(0,40));
                aItem->setFlags(0);
                ui->listWidgetStarred->addItem(aItem);

                document *mydou;
                mydou=new document(myMailList.list[i].info.senderID, myMailList.list[i].info.mailID, myMailList.list[i].info.title, myMailList.list[i].content);

                ui->listWidgetStarred->setItemWidget(aItem,mydou);

                int From = myMailList.list[i].info.senderID;
                int MailID = myMailList.list[i].info.mailID;
                QString Subject =  myMailList.list[i].info.title;
                QString Content =  myMailList.list[i].content;

                connect(mydou,SIGNAL( ItemGoDetail(int, int, QString, QString)),this,SLOT(readbin(int, int, QString, QString)));
            }
        }
        memset(&mymail,0,sizeof(mail));
        memcpy(mymail.controll.operation,"QUIT",8);
        socket->write((char*)&mymail,sizeof (mail));   //断开与服务器的连接


    });
    this->close();
    mynewhome->getmyname(myname);
    mynewhome->ui->pages_widget->setCurrentWidget(mynewhome->ui->pageStarred);
    mynewhome->show();
}

void home::on_btn_refresh_3_clicked()
{
    home *mynewhome= new home;
    ui=mynewhome->ui;

    memset(&mymail,0,sizeof(mail));
    memcpy(mymail.controll.operation,"R_SENT",8);

    socket->abort();
    socket->connectToHost(QHostAddress("127.0.0.1"),8888);
    mymail.user.id = myname.toInt();    // get hostID from storage
    socket->write((char*)&mymail,sizeof (mail));    // write socket for operation

    ui->listWidgetSent->clear();

    // receive from server
    connect(socket, &QTcpSocket::readyRead, [=]() {

        char buff[51208];
        memset(buff, 0, 51208);
        socket->read(buff, 51208);

        cout << "bin";

        struct mailList myMailList;
        memset(&myMailList, 0 ,sizeof(mailList));
        memcpy(&myMailList, buff, sizeof(struct mailList));

        ui->listWidgetSent->clear();

        for(int i=0; i<45; i++) {
            if(myMailList.list[i].info.mailID > 0 && myMailList.list[i].info.mailID < 300) {
                QListWidgetItem *aItem = new QListWidgetItem(ui->listWidgetSent);
                aItem->setSizeHint(QSize(0,40));
                aItem->setFlags(0);
                ui->listWidgetSent->addItem(aItem);

                document *mydou;
                mydou=new document(myMailList.list[i].info.senderID, myMailList.list[i].info.mailID, myMailList.list[i].info.title, myMailList.list[i].content);

                ui->listWidgetSent->setItemWidget(aItem,mydou);

                int From = myMailList.list[i].info.senderID;
                int MailID = myMailList.list[i].info.mailID;
                QString Subject =  myMailList.list[i].info.title;
                QString Content =  myMailList.list[i].content;

                connect(mydou,SIGNAL( ItemGoDetail(int, int, QString, QString)),this,SLOT(readbin(int, int, QString, QString)));
            }
        }
        memset(&mymail,0,sizeof(mail));
        memcpy(mymail.controll.operation,"QUIT",8);
        socket->write((char*)&mymail,sizeof (mail));   //断开与服务器的连接


    });
    this->close();
    mynewhome->getmyname(myname);
    mynewhome->ui->pages_widget->setCurrentWidget(mynewhome->ui->pageSent);
    mynewhome->show();
}

void home::on_btn_forward_2_clicked()
{
    memset(&mymail,0,sizeof(mail));
    memcpy(mymail.controll.operation,"BIN",8);

    socket->abort();
    socket->connectToHost(QHostAddress("127.0.0.1"),8888);

    mymail.info.mailID=mail_id;
    mymail.user.id=myname.toInt();
    memcpy(mymail.info.title,ui->readSubject->text().toStdString().data(),64);


    socket->write((char*)&mymail,sizeof(mail));
    QMessageBox::information(this, "Success", "Mail Moved to Bin.");
    memset(&mymail,0,sizeof(mail));
    memcpy(mymail.controll.operation,"QUIT",8);
    socket->write((char*)&mymail,sizeof (mail));   //断开与服务器的连接

}

void home::on_btn_readstarred_clicked()
{
    memset(&mymail,0,sizeof(mail));
    memcpy(mymail.controll.operation,"STAR",8);

    socket->abort();
    socket->connectToHost(QHostAddress("127.0.0.1"),8888);

    mymail.info.mailID=mail_id;
    mymail.user.id=myname.toInt();
    memcpy(mymail.info.title,ui->readSubject->text().toStdString().data(),64);

    socket->write((char*)&mymail,sizeof(mail));
    QMessageBox::information(this, "Success", "Mail is starred.");
    memset(&mymail,0,sizeof(mail));
    memcpy(mymail.controll.operation,"QUIT",8);
    socket->write((char*)&mymail,sizeof (mail));   //断开与服务器的连接
}

void home::on_btn_draft_clicked()
{
    memset(&mymail,0,sizeof(mail));
    memcpy(mymail.controll.operation,"DRAFT",8);
    socket->abort();
    socket->connectToHost(QHostAddress("127.0.0.1"),8888);
    QString Recipients=ui->lineRecipients->text();
    QString ccRecipients=ui->lineCc->text();
    QString bccRecipients=ui->lineRecip->text();
    QString title=ui->lineSubject->text();

    QString content = ui->TextEdit->toPlainText();
    memcpy(mymail.content,content.toStdString().data(),880);
    int i=1;
    while(i--)
    {
        QString sender=Recipients;
        mymail.info.receiverID = 0;
        mymail.info.senderID = myname.toInt();

        memcpy(mymail.info.title,title.toStdString().data(),64);
        socket->write((char*)&mymail,sizeof(mail));
    }

    memset(&mymail,0,sizeof(mail));

    memcpy(mymail.controll.operation,"QUIT",8);
    socket->write((char*)&mymail,sizeof (mail));   //断开与服务器的连接
}

void home::on_btn_search_clicked()
{
    home *mynewhome= new home;
    ui=mynewhome->ui;
    memset(&mymail,0,sizeof(mail));
    memcpy(mymail.controll.operation,"SEARCH",8);

    socket->abort();
    socket->connectToHost(QHostAddress("127.0.0.1"),8888);
    mymail.user.id = myname.toInt();    // get hostID from storage

    memcpy(mymail.info.title, ui->textEdit->toPlainText().toStdString().data(), 64);

    socket->write((char*)&mymail,sizeof (mail));    // write socket for operation

    ui->listWidgetInbox->clear();

    // receive from server
    connect(socket, &QTcpSocket::readyRead, [=]() {

        char buff[51208];
        memset(buff, 0, 51208);
        socket->read(buff, 51208);

        cout << "rec";
        struct mailList myMailList;
        memset(&myMailList, 0 ,sizeof(mailList));
        memcpy(&myMailList, buff, sizeof(struct mailList));

        ui->listWidgetInbox->clear();

        for(int i=0; i<45; i++) {
            if(myMailList.list[i].info.mailID > 0 && myMailList.list[i].info.mailID < 300) {

                QListWidgetItem *aItem = new QListWidgetItem(ui->listWidgetInbox);
                aItem->setSizeHint(QSize(0,35));
                aItem->setFlags(0);
                ui->listWidgetInbox->addItem(aItem);

                document *mydou;
                mydou=new document(myMailList.list[i].info.senderID, myMailList.list[i].info.mailID, myMailList.list[i].info.title, myMailList.list[i].content);
           //     ui->listWidgetInbox->setItemWidget(aItem,new document(myMailList.list[i].info.senderID, myMailList.list[i].info.title, myMailList.list[i].content));
                ui->listWidgetInbox->setItemWidget(aItem,mydou);

                QString Content =  myMailList.list[i].content;
                QString Subject =  myMailList.list[i].info.title;
                int From = myMailList.list[i].info.senderID;
                int MailID = myMailList.list[i].info.mailID;

                connect(mydou,SIGNAL( ItemGoDetail(int, int, QString, QString)),this,SLOT(readmail(int, int, QString, QString)));
            }
        }
        memset(&mymail,0,sizeof(mail));
        memcpy(mymail.controll.operation,"QUIT",8);
        socket->write((char*)&mymail,sizeof (mail));   //断开与服务器的连接

    });
    this->close();
    mynewhome->getmyname(myname);
    mynewhome->ui->pages_widget->setCurrentWidget(mynewhome->ui->pageInbox);
    mynewhome->show();
}

void home::on_btn_replybot_clicked()
{
    QString replyid= ui->From->text();
    ui->pages_widget->setCurrentWidget(ui->pageCompose);
    ui->lineRecipients->setText(replyid);

    ui->TextEdit->setText("");
    ui->lineSubject->setText("");

}

void home::on_btn_topreply_clicked()
{
    QString replyid= ui->From->text();
    ui->pages_widget->setCurrentWidget(ui->pageCompose);
    //    ui->readContent->setText("");
    //    ui->readSubject->setText("");
    ui->lineRecipients->setText(replyid);
//    ui->readContent->
    QString cont = ui->readContent->toPlainText();
    ui->TextEdit->setText(cont);
    ui->lineSubject->setText(ui->readSubject->text());
}

void home::on_fontComboBox_currentFontChanged(const QFont &f)
{
    ui->TextEdit->setCurrentFont(f);
    ui->TextEdit->setFocus();
}

