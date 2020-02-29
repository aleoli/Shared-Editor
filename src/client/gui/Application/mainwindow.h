/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

#include "../login/login.h"
#include "../registration/record.h"
#include "../registration/edit.h"
#include "../browser/docsbrowser.h"
#include "../editor/texteditor.h"
#include <list>
#include <QActionGroup>
#include "../../../File.h"
#include <QDockWidget>

class Manager;
using namespace std;


//QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
} // namespace Ui




class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    QAction *actionregister;
    QAction *actionLogin;
    QAction *actionlogout;
    QAction *actionShare;
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionExport;
    QAction *actionRename;
    QAction *actionCreate_a_copy;
    QAction *actionGo_back;
    QAction *actionBold;
    QAction *actionItalic;
    QAction *actionULined;
    QAction *actionPrevious_ctrl_z;
    QAction *actionRepeat_ctrl_y;
    QAction *actionCut_ctrl_x;
    QAction *actionCopy_ctrl_c;
    QAction *actionPaste_ctrl_v;
    QAction *actionGo_forward;
    //QWidget *centralWidget;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QWidget *page_2;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menumenu;
    QMenu *menuFile;
    QMenu *menuFormato;
    QMenu *menuTesto;
    QMenu *menuModifica;
    QToolBar *toolBar;

    void setupUi(QMainWindow *GMainWindow);
    void retranslateUi(QMainWindow *GMainWindow);

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void stackedWidgetInit();
    void openedNewPage(QObject q);

    Ui::MainWindow *getUi() const;
    Login* getLogin() const;
    Record* getRecord() const;
    DocsBrowser* getDocsBrowser() const;
    TextEditor* getTextEditor() const;
    void setManagerClass(Manager *m);
    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget *w);
    int getCurrentIndex();
    QWidget* getCurrentWidget();
    int indOf(QWidget *q);
    void showFeasibleActions();
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
    bool logged;
    //Manager class
    Manager *m;
    //metodi per mostrare i vari ui widgets

    void enumerateMenu(QMenu *menu);
    QActionGroup *loginFActions = new QActionGroup{this};;
    QActionGroup *recordFActions = new QActionGroup{this};;
    QActionGroup *texteditFActions = new QActionGroup{this};;
    QActionGroup *docsBrowsFActions = new QActionGroup{this};;
    QActionGroup *menuActions = new QActionGroup{this};;
    int centralWidgetIndex = -1;
public:
    int getCentralWidgetIndex() const;

private:
    void _newCentral();
    QMenu *menu;
    void createMenu();
    void _setCentral(QWidget *w);
    void checkRecord();

    list<QWidget *> history;
    list<QWidget *> next;


public slots:

    //MainWindow* getMW(){return this;}
    void showLoginPage();
    void showRecordPage();
    void showTextEditorPage();
    void showDocsBrowserPage();
    void showEditPage();
    void back();
    void forward();

private slots:
    void userIsLogged(bool res);
    void on_actionNew_triggered();
    void on_actionShare_triggered();
    void on_actionExport_triggered();
    void on_actionOpen_triggered();
    void on_actionCreate_a_copy_triggered();
    void on_actionGo_back_triggered();//fatta e controllata
    void on_actionregister_triggered();
    void on_actionlogout_triggered();
    void on_actionGo_forward_triggered();//fatta e controllata
    void on_actionLogin_triggered();

signals:

    void isUserLogged();
    void newDoc();
    void openDocument();
    void shareDocument();
    void createDocumentCopy();
    void exportDocument();
    //void openedNewPage();
    void goBack();
    void goForward();
};

//QT_END_NAMESPACE
#endif // MAINWINDOW_H
