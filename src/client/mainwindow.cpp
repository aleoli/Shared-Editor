#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    stackedWidgetInit();
    connectWidgets();
    showTextEditorPage();
}
MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::stackedWidgetInit(){
    login = new Login(ui->stackedWidget);
    ui->stackedWidget->addWidget(login);
    record = new Record(ui->stackedWidget);
    ui->stackedWidget->addWidget(record);
    textEditor = new TextEditor(ui->stackedWidget);
    ui->stackedWidget->addWidget(textEditor);
    docsBrowser = new DocsBrowser(ui->stackedWidget);
    ui->stackedWidget->addWidget(docsBrowser);
    ui->stackedWidget->setCurrentWidget(login);
}

void MainWindow::connectWidgets(){
    //login possibilities to change page (login successful -> docs browser | wanna register -> register )
    connect(login, SIGNAL(login_successful()), this, SLOT(userLogIn()));
    connect(login, SIGNAL(showRecordPage()), this, SLOT(showRecordPage()));

    //record possibilities to change page (go back to login page -> login page| successful registered -> docs browser)
    connect(record, SIGNAL(rec_successful()), this, SLOT(userLogIn()));
    connect(record, SIGNAL(showLoginPage()), this, SLOT(showLoginPage()));

    //docsBrowser possibilities to change page (go back to login page -> login page| open document -> text Editor)
    connect(docsBrowser, SIGNAL(open_file()), this, SLOT(showTextEditorPage()));
    connect(docsBrowser, SIGNAL(showLoginPage()), this, SLOT(userLogOut()));

    //textEditor possibilities to change page (go back to login page -> login page| back to documentBrowser -> docBrowser)
    connect(textEditor, SIGNAL(showTextBrowserPage()), this, SLOT(showTextBrowserPage()));
    connect(textEditor, SIGNAL(showLoginPage()), this, SLOT(userLogOut()));

}

void MainWindow::showLoginPage(){
    ui->stackedWidget->setCurrentWidget(login);
}

void MainWindow::showRecordPage(){
    ui->stackedWidget->setCurrentWidget(record);
}


void MainWindow::showTextEditorPage(){
    ui->stackedWidget->setCurrentWidget(textEditor);
}


void MainWindow::showDocsBrowserPage(){
    ui->stackedWidget->setCurrentWidget(docsBrowser);
}

void MainWindow::userLogIn(){
    //*user=login->getUser();
    if(ui->stackedWidget->currentWidget()==login){
        showDocsBrowserPage();
    }
}
void MainWindow::userLogOut(){
    this->user=nullptr;
    if(ui->stackedWidget->currentWidget()==docsBrowser||ui->stackedWidget->currentWidget()==textEditor){
        showLoginPage();
    }
}
