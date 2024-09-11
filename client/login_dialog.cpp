#include "login_dialog.h"
#include "ui_login_dialog.h"
#include<QDebug>
#include"http_mgr.h"
#include"tcpmgr.h"
login_dialog::login_dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::login_dialog)
{
    ui->setupUi(this);
    connect(ui->reg_btn,&QPushButton::clicked,this,&login_dialog::switch_reg);
    ui->passwd_edit->setEchoMode(QLineEdit::Password);
    ui->forget_label->SetState("normal","hover","","selected","selected_hover","");

    connect(ui->forget_label, &ClickedLabel::clicked, this, &login_dialog::slot_forget_pwd);
    initHttpHandlers();
    //连接登录回包信号
    connect(http_mgr::get_instance().get(), &http_mgr::sig_login_mod_finish, this,
            &login_dialog::slot_login_mod_finish);

    //连接tcp连接请求的信号和槽函数
    connect(this, &login_dialog::sig_connect_tcp, TcpMgr::get_instance().get(), &TcpMgr::slot_tcp_connect);
    //连接tcp管理者发出的连接成功信号
    connect(TcpMgr::get_instance().get(), &TcpMgr::sig_con_success, this, &login_dialog::slot_tcp_con_finish);
    //连接tcp管理者发出的连接失败信号
    connect(TcpMgr::get_instance().get(), &TcpMgr::sig_login_failed, this, &login_dialog::slot_login_failed);
}

login_dialog::~login_dialog()
{
    qDebug()<<"delet login dialog"<<Qt::endl;
    delete ui;
}

void login_dialog::initHttpHandlers()
{
    //注册获取登录回包逻辑
    _handlers.insert(ReqId::ID_LOGIN_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            enableBtn(true);
            return;
        }
        auto email=jsonObj["email"].toString();

        //发送信号通知tcpmgr发送长连接
        ServerInfo si;
        si.Uid=jsonObj["uid"].toInt();
        si.Host=jsonObj["host"].toString();
        si.Port=jsonObj["port"].toString();
        si.Token=jsonObj["token"].toString();

        _uid=si.Uid;
        _token=si.Token;
        qDebug()<<"email is "<<email<<" uid is "<<si.Uid<<" host is "<<si.Host<<" port is "<<si.Port<<" token is "<<si.Token;
        emit sig_connect_tcp(si);

        // auto user = jsonObj["user"].toString();
        // showTip(tr("登录成功"), true);
        // qDebug()<< "user is " << user ;
    });
}

bool login_dialog::checkUserValid()
{
    auto user = ui->user_edit->text();
    if(user.isEmpty()){
        qDebug() << "User empty " ;
        AddTipErr(TipErr::TIP_EMAIL_ERR,tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_EMAIL_ERR);

    return true;
}

bool login_dialog::checkPwdValid()
{
    auto pwd = ui->passwd_edit->text();
    if(pwd.length() < 6 || pwd.length() > 15){
        qDebug() << "Pass length invalid";
        AddTipErr(TipErr::TIP_PWD_ERR,tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*.]{6,15}$");
    bool match = regExp.match(pwd).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    return true;
}

void login_dialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips,false);
}

void login_dialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
        ui->error_tip->clear();
        return ;
    }
    showTip(_tip_errs.first(),false);
}

void login_dialog::showTip(QString str, bool b_ok)
{
    if(b_ok){
        ui->error_tip->setProperty("state","normal");
    }else{
        ui->error_tip->setProperty("state","err");
    }

    ui->error_tip->setText(str);

    repolish(ui->error_tip);
}

// void login_dialog::initHead()
// {

// }
bool login_dialog::enableBtn(bool enabled){
    ui->login_pushButton->setEnabled(enabled);
    ui->reg_btn->setEnabled(enabled);
    return true;
}

void login_dialog::slot_forget_pwd()
{
    qDebug()<<"slot forget pwd"<<Qt::endl;
    emit switch_reset();
}

void login_dialog::on_login_pushButton_clicked()
{
    qDebug()<<"login btn clicked";
    if(checkUserValid() == false){
        return;
    }

    if(checkPwdValid() == false){
        return ;
    }
    enableBtn(false);
    auto user = ui->user_edit->text();
    auto pwd = ui->passwd_edit->text();
    //发送http请求登录
    QJsonObject json_obj;
    json_obj["user"] = user;
    json_obj["passwd"] = xorString(pwd);
    http_mgr::get_instance()->post_httpreq(QUrl(gate_url_prefix+"/user_login"),
                                        json_obj, ReqId::ID_LOGIN_USER,Modules::LOGINMOD);
}

void login_dialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"),false);
        return;
    }

    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    //json解析错误
    if(jsonDoc.isNull()){
        showTip(tr("json解析错误"),false);
        return;
    }

    //json解析错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        return;
    }


    //调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());

    return;
}

void login_dialog::slot_tcp_con_finish(bool bsuccess)
{
    if(bsuccess){
        showTip(tr("聊天服务连接成功，正在登录..."),true);
        QJsonObject jsonObj;
        jsonObj["uid"] = _uid;
        jsonObj["token"] = _token;

        QJsonDocument doc(jsonObj);
        QByteArray jsonString = doc.toJson(QJsonDocument::Indented);

        //发送tcp请求给chat server
        emit TcpMgr::get_instance()->sig_send_data(ReqId::ID_CHAT_LOGIN, jsonString);

    }else{
        showTip(tr("网络异常"),false);
        enableBtn(true);
    }
}

void login_dialog::slot_login_failed(int)
{
    QString result=QString("登录失败，err is %1").arg(errno);
    showTip(result,false);
    enableBtn(true);

}



