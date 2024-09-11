#ifndef REGISTER_DIALOG_H
#define REGISTER_DIALOG_H

#include <QDialog>
#include"globe.h"
namespace Ui {
class register_dialog;
}

class register_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit register_dialog(QWidget *parent = nullptr);
    ~register_dialog();

private slots:
    void on_get_code_clicked(); //获取邮箱验证码
    void slot_reg_mod_finish(ReqId id , QString res,ErrorCodes err);
    void on_confirm_clicked();

    void on_return_btn_clicked();
    // void on_cancel_btn_clicked();

    void on_cancel_clicked();

private:
    void initHttpHandlers();
    void show_tip(QString str,bool b_ok);
    bool checkUserValid();
    bool checkPassValid();
    bool checkEmailValid();
    bool checkVarifyValid();
    bool checkConfirmValid();
    void AddTipErr(TipErr te, QString tips);
    void DelTipErr(TipErr te);

    void ChangeTipPage();

    Ui::register_dialog *ui;
    QMap<ReqId,std::function<void(const QJsonObject&)>> handlers;
    QMap<TipErr, QString> _tip_errs;

    QTimer * _countdown_timer;
    int _countdown;

signals:
    void sigSwitchLogin();
};

#endif // REGISTER_DIALOG_H
