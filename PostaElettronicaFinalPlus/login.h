#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include "regster.h"
#include "class.h"
#include "home.h"
#include <QHostAddress>
#include <QMessageBox>
#include <QTcpSocket>
#include <unistd.h>

namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

private slots:
    void on_pushButton_clicked();

    void reshow();

    void on_pushButton_2_clicked();

signals:
        void sendname(QString);
private:
    Ui::login *ui;
    regster * myregster;
    home * myhome;
    QTcpSocket * socket;
};

#endif // LOGIN_H
