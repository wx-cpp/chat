#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>
#include"globe.h"

namespace Ui {
class login_dialog;
}

class login_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit login_dialog(QWidget *parent = nullptr);
    ~login_dialog();

private:
    void initHttpHandlers();
    // void initHead();
    Ui::login_dialog *ui;
    bool checkUserValid();
    bool checkPwdValid();
    void AddTipErr(TipErr te, QString tips);
    void DelTipErr(TipErr te);
    QMap<TipErr, QString> _tip_errs;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    void showTip(QString str,bool b_ok);
    bool enableBtn(bool enabled);

    int _uid;
    QString _token;
signals:
    void switch_reg();
    void switch_reset();
    void sig_connect_tcp(ServerInfo);

public slots:
    void slot_forget_pwd();
private slots:
    void on_login_pushButton_clicked();
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);
    void slot_tcp_con_finish(bool bsuccess);
    void slot_login_failed(int);
};

#endif // LOGIN_DIALOG_H
