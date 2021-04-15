#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>
#include <QListWidgetItem>
#include "home.h"

namespace Ui {
class document;
}

class document : public QWidget, public QListWidgetItem
{
    Q_OBJECT

public:
    explicit document(QWidget *parent = 0);
    document(int From, int mailID, const char* Subject, const char* Content, QWidget *parent = 0);
    ~document();

private slots:
    void on_btn_click_clicked();

    void on_MailID_customContextMenuRequested(const QPoint &pos);

signals:
    void ItemGoDetail(int From, int mailID, QString Subject, QString Content);

private:
    Ui::document *ui;
    //Detail detail
    // to read page(DeTail
    home *myhome;
};

#endif // DOCUMENT_H
