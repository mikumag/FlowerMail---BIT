#ifndef REGSTER_H
#define REGSTER_H

#include <QWidget>
#include "class.h"
#include <QHostAddress>
#include <QMessageBox>
#include <QTcpSocket>
#include <unistd.h>

namespace Ui {
class regster;
}

class regster : public QWidget
{
    Q_OBJECT

public:
    explicit regster(QWidget *parent = nullptr);
    ~regster();

private slots:
    void on_pushButton_clicked();

    void on_line_lastName_cursorPositionChanged(int arg1, int arg2);

signals:
    void sendsignal();

private:
    Ui::regster *ui;
    QTcpSocket * socket;
};

#endif // REGSTER_H
