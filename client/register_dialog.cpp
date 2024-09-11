#include "register_dialog.h"
#include "ui_register_dialog.h"
#include"globe.h"
#include"http_mgr.h"
#include<QDebug>

register_dialog::register_dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::register_dialog),_countdown(5)
{
    ui->setupUi(this);

    ui->passwd_edit->setEchoMode(QLineEdit::Password);
    ui->wd_edit->setEchoMode(QLineEdit::Password);

    ui->error_tip->setProperty("state","normal");//设置属性
    repolish(ui->error_tip);

    connect(http_mgr::get_instance().get(),&http_mgr::sig_reg_mod_finish,this,&register_dialog::slot_reg_mod_finish);

    initHttpHandlers();
    ui->error_tip->clear();

    connect(ui->user_edit,&QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });

    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });

    connect(ui->passwd_edit, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });

    connect(ui->wd_edit, &QLineEdit::editingFinished, this, [this](){
        checkConfirmValid();
    });

    connect(ui->varify_edit, &QLineEdit::editingFinished, this, [this](){
        checkVarifyValid();
    });

    ui->passwd_visible->setCursor(Qt::PointingHandCursor);
    ui->wd_visible->setCursor(Qt::PointingHandCursor);

    ui->passwd_visible->SetState("unvisible","unvisible_hover","","visible",
                               "visible_hover","");

    ui->wd_visible->SetState("unvisible","unvisible_hover","","visible",
                                  "visible_hover","");

//连接点击事件

    connect(ui->passwd_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->passwd_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->passwd_edit->setEchoMode(QLineEdit::Password);
        }else{
                ui->passwd_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
        });

    connect(ui->wd_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->wd_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->wd_edit->setEchoMode(QLineEdit::Password);
        }else{
                ui->wd_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
        });

    //创建定时器
    _countdown_timer=new QTimer(this);
    // 连接信号和槽
    connect(_countdown_timer, &QTimer::timeout, [this](){
        if(_countdown==0){
            _countdown_timer->stop();
            emit sigSwitchLogin();
            return;
        }
        _countdown--;
        auto str = QString("注册成功，%1 s后返回登录").arg(_countdown);
        ui->tip_lb->setText(str);
    });
}

register_dialog::~register_dialog()
{
    qDebug()<<"delet register dialog"<<Qt::endl;
    delete ui;
}


void register_dialog::on_get_code_clicked()
{
    auto email=ui->email_edit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");//邮箱正则表达式
    bool match=regex.match(email).hasMatch();
    if(match){
        //发送http验证码
        QJsonObject json_obj;
        json_obj["email"]=email;
        http_mgr::get_instance()->post_httpreq(QUrl(gate_url_prefix+"/get_varifycode"),json_obj,ReqId::ID_GET_VARIFY_CODE,Modules::REGISTERMOD);

    }else{
        show_tip(tr("邮箱不正确"),false);
    }

}

void register_dialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err!=ErrorCodes::SUCCESS){
        show_tip(tr("网络请求错误"),false);
        return ;
    }

    //解析json字符串，res转换为obytearray
    QJsonDocument jsonDoc=QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isEmpty()){
        show_tip(tr("Json解析失败"),false);
        return;
    }
    //json解析错误
    if(!jsonDoc.isObject()){
        show_tip(tr("Json解析失败"),false);
        return;
    }

    handlers[id](jsonDoc.object());
    return ;
}

void register_dialog::initHttpHandlers()
{
    //注册获取验证码回包的逻辑
    handlers.insert(ReqId::ID_GET_VARIFY_CODE,[this](const QJsonObject& json_obj){
        int error = json_obj["error"].toInt();
        if(error!=ErrorCodes::SUCCESS){
            show_tip(tr("参数错误"),false);
            return;
        }
        auto email=json_obj["email"].toString();
        show_tip(tr("验证码已发送"),true);
        qDebug()<<"email is "<<email;
    });

    //注册用户回包逻辑
    handlers.insert(ReqId::ID_REG_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            show_tip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        show_tip(tr("用户注册成功"), true);
        qDebug()<< "email is " << email ;
        ChangeTipPage();
    });
}

void register_dialog::show_tip(QString str, bool b_ok)
{
    if(b_ok){
        ui->error_tip->setProperty("state","normal");
    }else{
        ui->error_tip->setProperty("state","err");
    }

    ui->error_tip->setText(str);
    // ui->error_tip->setProperty("state","err");
    repolish(ui->error_tip);
}

//添加错误提示
void register_dialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    show_tip(tips, false);
}

//移除错误提示
void register_dialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
        ui->error_tip->clear();
        return;
    }

    show_tip(_tip_errs.first(), false);
}

void register_dialog::ChangeTipPage()
{
    _countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);

    // 启动定时器，设置间隔为1000毫秒（1秒）
    _countdown_timer->start(1000);

}

bool register_dialog::checkUserValid()
{
    if(ui->user_edit->text() == ""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool register_dialog::checkPassValid()
{
    auto pass = ui->passwd_edit->text();
    // auto confirm=ui->wd_edit->text();
    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);


    return true;
}



bool register_dialog::checkEmailValid()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }

    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool register_dialog::checkVarifyValid()
{
    auto pass = ui->varify_edit->text();
    if(pass.isEmpty()){
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

bool register_dialog::checkConfirmValid()
{
    auto pass = ui->passwd_edit->text();
    auto confirm=ui->wd_edit->text();

    if(confirm!=pass){
        AddTipErr(TipErr::TIP_PWD_CONFIRM,tr("密码与确认密码不匹配"));
        return false;
    }
    else{
        DelTipErr(TipErr::TIP_PWD_CONFIRM);
    }

    return true;
}


void register_dialog::on_confirm_clicked()
{
    // if(ui->user_edit->text() == ""){
    //     show_tip(tr("用户名不能为空"), false);
    //     return;
    // }

    // if(ui->email_edit->text() == ""){
    //     show_tip(tr("邮箱不能为空"), false);
    //     return;
    // }

    // if(ui->passwd_edit->text() == ""){
    //     show_tip(tr("密码不能为空"), false);
    //     return;
    // }

    // if(ui->wd_edit->text() == ""){
    //     show_tip(tr("确认密码不能为空"), false);
    //     return;
    // }

    // if(ui->wd_edit->text() != ui->passwd_edit->text()){
    //     show_tip(tr("密码和确认密码不匹配"), false);
    //     return;
    // }

    // if(ui->varify_edit->text() == ""){
    //     show_tip(tr("验证码不能为空"), false);
    //     return;
    // }

    bool valid = checkUserValid();
    if(!valid){
        return;
    }

    valid = checkEmailValid();
    if(!valid){
        return;
    }

    valid = checkPassValid();
    if(!valid){
        return;
    }

    valid = checkVarifyValid();
    if(!valid){
        return;
    }

    //发送http请求注册用户
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = xorString(ui->passwd_edit->text());
    json_obj["confirm"] = xorString(ui->wd_edit->text());
    json_obj["varifycode"] = ui->varify_edit->text();
    http_mgr::get_instance()->post_httpreq(QUrl(gate_url_prefix+"/user_register"),
                                        json_obj, ReqId::ID_REG_USER,Modules::REGISTERMOD);
}


void register_dialog::on_return_btn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}

// void register_dialog::on_cancel_btn_clicked()
// {
//     _countdown_timer->stop();
//     emit sigSwitchLogin();
// }


void register_dialog::on_cancel_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}

