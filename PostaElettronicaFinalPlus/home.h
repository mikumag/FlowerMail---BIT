#ifndef HOME_H
#define HOME_H

#include <QWidget>
#include <unistd.h>
#include <QFont>
#include <QListWidgetItem>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include "class.h"

namespace Ui {
class home;
}

class home : public QWidget
{
    Q_OBJECT

public:
    QTcpSocket * socket;

    class Mail
    {
    public:
        //Mail(QString From, QString Subject, QString Content);
        Mail(QString From, QString Subject, QString Content)
                : From(From), Subject(Subject), Content(Content)
            {
            }
        QString From;
        QString Subject;
        QString Content;


    };
    void getmyname(QString);

    static std::vector<Mail> mails;
    explicit home(QWidget *parent = nullptr);
    ~home();

private slots:
    void on_btn_compose_clicked();

    void on_btn_inbox_clicked();

    void on_btn_starred_clicked();

    void on_btn_sent_clicked();

    void on_btn_drafts_clicked();

    void on_btn_bin_clicked();

    void on_btn_send_clicked();

    void getname(QString);

    void on_btn_refresh_clicked();

    void on_listWidgetInbox_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_btn_help_clicked();

    void on_btn_logout_clicked();

    void readmail(int From, int mailID, QString Subject, QString Content);

    void readbin(int From, int mailID, QString Subject, QString Content);

    void on_btn_refresh_4_clicked();

    void on_fontComboBox_activated(const QString &arg1);

    void on_comboBox_2_activated(int index);

    void on_btn_refresh_5_clicked();

    void on_btn_refresh_2_clicked();

    void on_btn_refresh_3_clicked();

    void on_btn_forward_2_clicked();

    void on_btn_readstarred_clicked();

    void on_btn_draft_clicked();

    void on_btn_search_clicked();

    void on_btn_replybot_clicked();

    void on_btn_topreply_clicked();

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_comboBox_2_activated(const QString &arg1);

signals:
    void sendsignal();

private:
    Ui::home *ui;


};

#endif // HOME_H
