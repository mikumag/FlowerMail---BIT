#ifndef MAINOBJ_H
#define MAINOBJ_H

#include <QWidget>

#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>


//#include "draft_mail.h"
//#include "dustbin.h"
//#include "read_mail.h"
//#include "recieve_mail.h"
//#include "written_mail.h"
//#include "write_mail.h"

namespace Ui {
class mainobj;
}

class mainobj : public QWidget
{
    Q_OBJECT

public:
    explicit mainobj(QWidget *parent = nullptr);
    ~mainobj();

private slots:
//    void on_pushButton_5_clicked();

//    void on_pushButton_clicked();

//    void on_pushButton_2_clicked();

//    void on_pushButton_3_clicked();

//    void on_pushButton_4_clicked();

    void on_writebutton_clicked();

    void on_recievebutton_clicked();

    void on_sendbutton_clicked();

    void on_draftbutton_clicked();

    void on_dustbinbutton_clicked();

    void on_starbutton_clicked();

//    void on_testbutton_clicked();

//    void on_testbrowser_anchorClicked(const QUrl &arg1);

    void on_pushButton_clicked();

private:
    Ui::mainobj *ui;
//    draft_mail *mydraft;
//    dustbin *mydustbin;
//    read_mail * myread;
//    recieve_mail * myrecieve;
//    written_mail * mywritten;
//    write_mail * mywrite;
};

#endif // MAINOBJ_H
