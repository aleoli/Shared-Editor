#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "login.h"
#include "record.h"
#include "docsbrowser.h"
#include "texteditor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void stackedWidgetInit();
    void connectWidgets();

private:
    //vari widgets per le interfacce
    Ui::MainWindow *ui;
    Login *login;
    Record *record;
    DocsBrowser *docsBrowser;
    TextEditor *textEditor;

    //account utente loggato, null se non loggato
    Account *user=nullptr;

    //metodi per mostrare i vari ui widgets


public slots:
    void userLogIn();
    void userLogOut();
    void showLoginPage();
    void showRecordPage();
    void showTextEditorPage();
    void showDocsBrowserPage();
private slots:
    void on_actionPrint_triggered();
};

#endif // MAINWINDOW_H
