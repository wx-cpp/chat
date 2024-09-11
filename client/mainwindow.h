#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"login_dialog.h"
#include"register_dialog.h"
#include"resetdialog.h"
#include"chatdialog.h"
/****************************************************
*@file  mainwindow.h
*@brief 主窗口
****************************************************/
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void slot_switchreg();
    void slot_switchlogin();
    void slot_switchreset();
    void SlotSwitchLogin2();
    void SlotSwitchChat();
private:
    Ui::MainWindow *ui;
    login_dialog * login_dlg;
    register_dialog * reg_dlg;
    ResetDialog * _reset_dlg;
    ChatDialog* _chat_dlg;
};
#endif // MAINWINDOW_H
