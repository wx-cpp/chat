#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "resetdialog.h"
#include"tcpmgr.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    login_dlg = new login_dialog(this);
    login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(login_dlg);
    //login_dlg->show();

    //创建和注册消息连接
    connect(login_dlg,&login_dialog::switch_reg,this,&MainWindow::slot_switchreg);
    // reg_dlg = new register_dialog(this);

    //连接登录界面忘记密码信号
    connect(login_dlg, &login_dialog::switch_reset, this, &MainWindow::slot_switchreset);

    //连接聊天界面信号
    connect(TcpMgr::get_instance().get(),&TcpMgr::sig_swich_chatdlg, this, &MainWindow::SlotSwitchChat);

    // emit TcpMgr::get_instance()->sig_swich_chatdlg();//测试
    // //设置无边框，将创建和注册对话框嵌入mainwindow
    // login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    // reg_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    // reg_dlg->hide();
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::slot_switchreg()
{
    reg_dlg=new register_dialog(this);

    reg_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    //连接注册界面返回登录信号
    connect(reg_dlg, &register_dialog::sigSwitchLogin, this, &MainWindow::slot_switchlogin);
    setCentralWidget(reg_dlg);
    login_dlg->close();
    reg_dlg->show();
}

void MainWindow::slot_switchlogin()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    login_dlg = new login_dialog(this);
    login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(login_dlg);

    reg_dlg->hide();
    login_dlg->show();
    //连接登录界面注册信号
    connect(login_dlg, &login_dialog::switch_reg, this, &MainWindow::slot_switchreg);
    //连接登录界面忘记密码信号
    connect(login_dlg, &login_dialog::switch_reset, this, &MainWindow::slot_switchreset);
}

void MainWindow::slot_switchreset()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    _reset_dlg = new ResetDialog(this);
    _reset_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_reset_dlg);

    login_dlg->hide();
    _reset_dlg->show();
    //重置密码返回登录信号和槽函数
    connect(_reset_dlg, &ResetDialog::switchLogin, this, &MainWindow::SlotSwitchLogin2);
}

void MainWindow::SlotSwitchLogin2()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    login_dlg = new login_dialog(this);
    login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(login_dlg);

    _reset_dlg->hide();
    login_dlg->show();
    //连接登录界面忘记密码信号
    connect(login_dlg, &login_dialog::switch_reset, this, &MainWindow::slot_switchreset);
    //连接登录界面注册信号
    connect(login_dlg, &login_dialog::switch_reg, this, &MainWindow::slot_switchreg);


}

void MainWindow::SlotSwitchChat()
{
    _chat_dlg = new ChatDialog();
    _chat_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_chat_dlg);
    _chat_dlg->show();
    login_dlg->hide();
    this->setMinimumSize(QSize(1050,900));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}
