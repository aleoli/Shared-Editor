#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "docsbrowser.h"
#include <memory>
#include <iostream>
#include "manager.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connectWidgets();
    logged = false;
    if(!logged) {        // l'utente non è loggato
        this->showLoginPage();
    } else {
        this->showDocsBrowserPage();
    }
    qDebug("main window instantiated");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::back() {
    if(history.size()<=1) return;
    auto prev = this->history.back();
    if(!logged && (prev->inherits("TextEditor") || prev->inherits("DocsBrowser"))){
        history.clear();
        next.clear();
        this->showLoginPage();
    }
    this->history.pop_back();
    auto cur = this->centralWidget();
    //if is texteditor checkout the correct document
    cur->setParent(nullptr);
    this->next.push_back(cur);
    this->setCentralWidget(prev);
    this->_newCentral();
    this->showFeasibleActions();
}

void MainWindow::forward() {
    if(next.empty()) return;
    auto cur = this->centralWidget();
    cur->setParent(nullptr);
    this->history.push_back(cur);
    auto n = this->next.back();
    if(!logged && (n->inherits("TextEditor") || n->inherits("DocsBrowser"))){
        history.clear();
        next.clear();
        this->showLoginPage();
    }
    this->next.pop_back();
    this->setCentralWidget(n);
    this->_newCentral();
    this->showFeasibleActions();
}

void MainWindow::_setCentral(QWidget *w) {
    if(!logged && (w->inherits("TextEditor") || w->inherits("DocsBrowser"))){
        history.clear();
        next.clear();
        this->showLoginPage();
    }
    for(auto& item: this->next) {
        delete item;
    }
    this->next.clear();
    auto cur = this->centralWidget();
    if(cur) {
        cur->setParent(nullptr);
        this->history.push_back(cur);
    }
    this->setCentralWidget(w);
    this->_newCentral();
    this->showFeasibleActions();
}

void MainWindow::_newCentral(){
    if(this->centralWidget()->inherits("Login"))
        this->centralWidgetIndex = 0;
    else if(this->centralWidget()->inherits("Record"))
        this->centralWidgetIndex = 1;
    else if(this->centralWidget()->inherits("DocsBrowser"))
        this->centralWidgetIndex = 2;
    else if(this->centralWidget()->inherits("TextEditor"))
        this->centralWidgetIndex = 3;
}
void MainWindow::connectWidgets() {
    //auto connect
    connect(this, &MainWindow::goBack, this, &MainWindow::back);
    connect(this, &MainWindow::goForward, this, &MainWindow::forward);

    //manager signals

    // connetto i signal dei widget con gli slot di questa classe


    /*connect(login, &Login::loginRequest, this, &GuiWrapper::sendLoginRequest);
    connect(docsBrowser, &DocsBrowser::openFileRequest, this, &MainWindow::sendFileRequest);
    connect(docsBrowser, &DocsBrowser::newFileRequest, this, &MainWindow::sendNewFileRequest);
    */
}

void MainWindow::showFeasibleActions()
{
    /*
    foreach (QAction* action, menuActions->actions()){
        action->setCheckable(false);
    }
    switch(this->centralWidgetIndex){
        case 0: {//login
            foreach(QAction *action, loginFActions->actions())
                action->setEnabled(true);
            break;
        }
        case 1: {//record
            foreach(QAction *action, recordFActions->actions())
                action->setEnabled(true);
            break;
        }
        case 2: {//docsBrowser
            foreach(QAction *action, texteditFActions->actions())
                action->setEnabled(true);
            break;
        }
        case 3: {//textEditor
            foreach(QAction *action, docsBrowsFActions->actions())
                action->setEnabled(true);
            break;
        }

        default: {
            break;
        }
    }
    */
}

void MainWindow::enumerateMenu(QMenu *menu)
{
        /*
    foreach (QAction *action, menu->actions()) {
        if (action->isSeparator()) {
            qDebug("this action is a separator");
        } else if (action->menu()) {
            qDebug("action: %s", qUtf8Printable(action->text()));
            qDebug(">>> this action is associated with a submenu, iterating it recursively...");
            enumerateMenu(action->menu());
            qDebug("<<< finished iterating the submenu");
        } else {
            qDebug("action: %s", qUtf8Printable(action->text()));
            menuActions->addAction(action);
        }
    }
    */
}

void MainWindow::showLoginPage(){
    auto login = new Login{this};
    //login possibilities to change page (login successful -> docs browser | wanna register -> register )
    emit new_login_widget(login);
    /*connect(login, SIGNAL(access()), this, SLOT(checkLogin()));
    connect(login, &Login::showRecordPage, this, &MainWindow::showRecordPage);
    connect(login, SIGNAL(showRecordPage()), this, SLOT(showRecordPage()));
    this->_setCentral(login);*/
}

void MainWindow::showRecordPage(){
    auto record = new Record{this};
    //record possibilities to change page (go back to login page -> login page| successful registered -> docs browser)
    //connect(record, SIGNAL(rec_successful()), this, SLOT(checkRecord()));
    //connect(record, SIGNAL(showLoginPage()), this, SLOT(showLoginPage()));
    this->_setCentral(record);
}


void MainWindow::showTextEditorPage(){
    emit isUserLogged();
    if(this->logged){
        auto te = new TextEditor{this};
        //docsBrowser possibilities to change page (go back to login page -> login page| open document -> text Editor)
        //connect(docsBrowser, SIGNAL(open_file()), this, SLOT(showTextEditorPage()));
        //connect(docsBrowser, SIGNAL(showLoginPage()), this, SLOT(userLogOut()));

        //TODO
        //affiliare un oggetto di tipo document ?
        this->_setCentral(te);
    }
}

void MainWindow::userIsLogged(bool result){
    this->logged = result;
}

void MainWindow::showDocsBrowserPage(){
    emit isUserLogged();
    if(this->logged){
        auto db = new DocsBrowser{this};
        //textEditor possibilities to change page (go back to login page -> login page| back to documentBrowser -> docBrowser)
        //connect(textEditor, SIGNAL(showTextBrowserPage()), this, SLOT(showTextBrowserPage()));
        //connect(textEditor, SIGNAL(showLoginPage()), this, SLOT(userLogOut()));
        this->_setCentral(db);
    }
}

void MainWindow::showEditPage(){
    emit isUserLogged();
    if(this->logged){
        auto ed = new Edit{this};
        this->_setCentral(ed);
    }
}

void MainWindow::on_actionNew_triggered()
{
    emit newDoc();
    //showTextEditorPage();
}

void MainWindow::on_actionShare_triggered()
{
    emit shareDocument();
    //showDocLink(Document d);
}

void MainWindow::on_actionOpen_triggered()
{
    emit openDocument();
    //openDocument or goback to docs browser page?;
}

void MainWindow::on_actionExport_triggered()
{
    emit exportDocument();
    //createAndSavePDF(Document d);
}


void MainWindow::on_actionCreate_a_copy_triggered()
{
    emit createDocumentCopy();
    //createACopy(Document d);
}

void MainWindow::on_actionGo_back_triggered()
{
    emit this->goBack();
}

void MainWindow::setManagerClass(Manager *m){
    this->m=m;
}

void MainWindow::on_actionLogin_triggered()
{
    user=nullptr;
    showLoginPage();
}

void MainWindow::on_actionregister_triggered()
{
    user=nullptr;
    showRecordPage();
}

void MainWindow::on_actionlogout_triggered()
{
    user=nullptr;
    this->logged = false;
    showLoginPage();
}

void MainWindow::on_actionGo_forward_triggered()
{
    //nextPage();
    emit this->goForward();
}

void MainWindow::setupUi(QMainWindow *MainWindow){

        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1920, 1080);
        actionregister = new QAction(MainWindow);
        actionregister->setObjectName(QString::fromUtf8("actionregister"));
        actionLogin = new QAction(MainWindow);
        actionLogin->setObjectName(QString::fromUtf8("actionLogin"));
        actionlogout = new QAction(MainWindow);
        actionlogout->setObjectName(QString::fromUtf8("actionlogout"));
        actionShare = new QAction(MainWindow);
        actionShare->setObjectName(QString::fromUtf8("actionShare"));
        actionNew = new QAction(MainWindow);
        actionNew->setObjectName(QString::fromUtf8("actionNew"));
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionExport = new QAction(MainWindow);
        actionExport->setObjectName(QString::fromUtf8("actionExport"));
        actionRename = new QAction(MainWindow);
        actionRename->setObjectName(QString::fromUtf8("actionRename"));
        actionCreate_a_copy = new QAction(MainWindow);
        actionCreate_a_copy->setObjectName(QString::fromUtf8("actionCreate_a_copy"));
        actionGo_back = new QAction(MainWindow);
        actionGo_back->setObjectName(QString::fromUtf8("actionGo_back"));
        actionGo_back->setEnabled(true);
        actionBold = new QAction(MainWindow);
        actionBold->setObjectName(QString::fromUtf8("actionBold"));
        actionItalic = new QAction(MainWindow);
        actionItalic->setObjectName(QString::fromUtf8("actionItalic"));
        actionULined = new QAction(MainWindow);
        actionULined->setObjectName(QString::fromUtf8("actionULined"));
        actionPrevious_ctrl_z = new QAction(MainWindow);
        actionPrevious_ctrl_z->setObjectName(QString::fromUtf8("actionPrevious_ctrl_z"));
        actionRepeat_ctrl_y = new QAction(MainWindow);
        actionRepeat_ctrl_y->setObjectName(QString::fromUtf8("actionRepeat_ctrl_y"));
        actionCut_ctrl_x = new QAction(MainWindow);
        actionCut_ctrl_x->setObjectName(QString::fromUtf8("actionCut_ctrl_x"));
        actionCopy_ctrl_c = new QAction(MainWindow);
        actionCopy_ctrl_c->setObjectName(QString::fromUtf8("actionCopy_ctrl_c"));
        actionPaste_ctrl_v = new QAction(MainWindow);
        actionPaste_ctrl_v->setObjectName(QString::fromUtf8("actionPaste_ctrl_v"));
        actionGo_forward = new QAction(MainWindow);
        actionGo_forward->setObjectName(QString::fromUtf8("actionGo_forward"));
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1126, 21));
        menumenu = new QMenu(menuBar);
        menumenu->setObjectName(QString::fromUtf8("menumenu"));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuFormato = new QMenu(menuBar);
        menuFormato->setObjectName(QString::fromUtf8("menuFormato"));
        menuTesto = new QMenu(menuFormato);
        menuTesto->setObjectName(QString::fromUtf8("menuTesto"));
        menuModifica = new QMenu(menuBar);
        menuModifica->setObjectName(QString::fromUtf8("menuModifica"));
        MainWindow->setMenuBar(menuBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);


        menuBar->addAction(menumenu->menuAction());
        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuModifica->menuAction());
        menuBar->addAction(menuFormato->menuAction());

        menumenu->addAction(actionGo_back);
        loginFActions->addAction(actionGo_back);
        recordFActions->addAction(actionGo_back);
        docsBrowsFActions->addAction(actionGo_back);
        texteditFActions->addAction(actionGo_back);

        menumenu->addAction(actionGo_forward);
        loginFActions->addAction(actionGo_forward);
        recordFActions->addAction(actionGo_forward);
        docsBrowsFActions->addAction(actionGo_forward);
        texteditFActions->addAction(actionGo_forward);

        menumenu->addSeparator();

        menumenu->addAction(actionregister);
        loginFActions->addAction(actionregister);
        docsBrowsFActions->addAction(actionregister);
        texteditFActions->addAction(actionregister);

        menumenu->addAction(actionLogin);
        recordFActions->addAction(actionLogin);
        docsBrowsFActions->addAction(actionLogin);
        texteditFActions->addAction(actionLogin);

        menumenu->addSeparator();
        menumenu->addAction(actionlogout);
        docsBrowsFActions->addAction(actionlogout);
        texteditFActions->addAction(actionlogout);

        menuFile->addAction(actionShare);
        docsBrowsFActions->addAction(actionShare);
        texteditFActions->addAction(actionShare);

        menuFile->addSeparator();

        menuFile->addAction(actionNew);
        docsBrowsFActions->addAction(actionNew);
        texteditFActions->addAction(actionNew);

        menuFile->addAction(actionOpen);
        docsBrowsFActions->addAction(actionOpen);
        texteditFActions->addAction(actionOpen);

        menuFile->addSeparator();

        menuFile->addAction(actionExport);
        docsBrowsFActions->addAction(actionExport);
        texteditFActions->addAction(actionExport);

        menuFile->addAction(actionRename);
        docsBrowsFActions->addAction(actionRename);
        texteditFActions->addAction(actionRename);

        menuFile->addAction(actionCreate_a_copy);
        docsBrowsFActions->addAction(actionCreate_a_copy);
        texteditFActions->addAction(actionCreate_a_copy);

        menuFormato->addAction(menuTesto->menuAction());
        menuFormato->addSeparator();

        menuTesto->addAction(actionBold);
        texteditFActions->addAction(actionBold);
        menuTesto->addAction(actionItalic);
        texteditFActions->addAction(actionItalic);
        menuTesto->addAction(actionULined);
        texteditFActions->addAction(actionULined);

        menuModifica->addAction(actionPrevious_ctrl_z);
        docsBrowsFActions->addAction(actionPrevious_ctrl_z);
        texteditFActions->addAction(actionPrevious_ctrl_z);

        menuModifica->addAction(actionRepeat_ctrl_y);
        docsBrowsFActions->addAction(actionRepeat_ctrl_y);
        texteditFActions->addAction(actionRepeat_ctrl_y);

        menuModifica->addSeparator();
        menuModifica->addAction(actionCut_ctrl_x);
        docsBrowsFActions->addAction(actionCut_ctrl_x);
        texteditFActions->addAction(actionCut_ctrl_x);

        menuModifica->addAction(actionCopy_ctrl_c);
        docsBrowsFActions->addAction(actionCopy_ctrl_c);
        texteditFActions->addAction(actionCopy_ctrl_c);

        menuModifica->addAction(actionPaste_ctrl_v);
        docsBrowsFActions->addAction(actionPaste_ctrl_v);
        texteditFActions->addAction(actionPaste_ctrl_v);


        //stackedWidget->setCurrentIndex(0);

        //group actions to handle their possibilities
        enumerateMenu(menumenu);
        enumerateMenu(menuFile);
        enumerateMenu(menuModifica);
        enumerateMenu(menuFormato);

        //doc browser feasible actions
        /*foreach(QAction *action,menuBar->actions()){
            qDebug("action: %s\n",qUtf8Printable(action->text()));
            docsBrowsFActions->addAction(action);
        }*

        //text editor feasible actions
        foreach(QAction *action,menuBar->actions()){
            texteditFActions->addAction(action);
        }
        */

        QMetaObject::connectSlotsByName(MainWindow);
        retranslateUi(MainWindow);

    } // setupUi

void MainWindow::retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionregister->setText(QCoreApplication::translate("MainWindow", "register", nullptr));
        actionLogin->setText(QCoreApplication::translate("MainWindow", "login", nullptr));
        actionlogout->setText(QCoreApplication::translate("MainWindow", "logout", nullptr));
        actionShare->setText(QCoreApplication::translate("MainWindow", "Share", nullptr));
        actionNew->setText(QCoreApplication::translate("MainWindow", "New", nullptr));
        actionOpen->setText(QCoreApplication::translate("MainWindow", "Open", nullptr));
        actionExport->setText(QCoreApplication::translate("MainWindow", "Export", nullptr));
        actionRename->setText(QCoreApplication::translate("MainWindow", "Rename", nullptr));
        actionCreate_a_copy->setText(QCoreApplication::translate("MainWindow", "Create a copy", nullptr));
        actionGo_back->setText(QCoreApplication::translate("MainWindow", "Go back", nullptr));
        actionBold->setText(QCoreApplication::translate("MainWindow", "Bold", nullptr));
        actionItalic->setText(QCoreApplication::translate("MainWindow", "Italic", nullptr));
        actionULined->setText(QCoreApplication::translate("MainWindow", "ULined", nullptr));
        actionPrevious_ctrl_z->setText(QCoreApplication::translate("MainWindow", "Previous (ctrl+z)", nullptr));
#if QT_CONFIG(shortcut)
        actionPrevious_ctrl_z->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Z", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRepeat_ctrl_y->setText(QCoreApplication::translate("MainWindow", "Repeat (ctrl+y)", nullptr));
#if QT_CONFIG(shortcut)
        actionRepeat_ctrl_y->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Y", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCut_ctrl_x->setText(QCoreApplication::translate("MainWindow", "Cut (ctrl+x)", nullptr));
#if QT_CONFIG(shortcut)
        actionCut_ctrl_x->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+X", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCopy_ctrl_c->setText(QCoreApplication::translate("MainWindow", "Copy (ctrl+c)", nullptr));
#if QT_CONFIG(shortcut)
        actionCopy_ctrl_c->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+C", nullptr));
#endif // QT_CONFIG(shortcut)
        actionPaste_ctrl_v->setText(QCoreApplication::translate("MainWindow", "Paste (ctrl+v)", nullptr));
#if QT_CONFIG(shortcut)
        actionPaste_ctrl_v->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+V", nullptr));
#endif // QT_CONFIG(shortcut)
        actionGo_forward->setText(QCoreApplication::translate("MainWindow", "Go forward", nullptr));
        menumenu->setTitle(QCoreApplication::translate("MainWindow", "Menu", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuFormato->setTitle(QCoreApplication::translate("MainWindow", "Formato", nullptr));
        menuTesto->setTitle(QCoreApplication::translate("MainWindow", "Testo", nullptr));
        menuModifica->setTitle(QCoreApplication::translate("MainWindow", "Modify", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
    }

int MainWindow::getCentralWidgetIndex() const {
    return centralWidgetIndex;
}


// retranslateUi


/*void MainWindow::createMenu(){
   menu = menuBar()->addMenu(tr("&File"));
   QToolBar *fileToolBar = addToolBar(tr("File"));
   const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
   QAction *newAct = new QAction(newIcon, tr("&New"), this);
   newAct->setShortcuts(QKeySequence::New);
   newAct->setStatusTip(tr("Create a new file"));
   //connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
   menu->addAction(newAct);
   fileToolBar->addAction(newAct);

   const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
   QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
   openAct->setShortcuts(QKeySequence::Open);
   openAct->setStatusTip(tr("Open an existing file"));
   connect(openAct, &QAction::triggered, this, &MainWindow::open);
   menu->addAction(openAct);
   fileToolBar->addAction(openAct);
   ...
   QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
   aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));

}*/

/*
Ui::MainWindow *MainWindow::getUi() const
{
    return ui;
}

Login *MainWindow::getLogin() const
{
    return this->login;
}

Record *MainWindow::getRecord() const
{
    return record;
}

DocsBrowser *MainWindow::getDocsBrowser() const
{
    return docsBrowser;
}

TextEditor *MainWindow::getTextEditor() const
{
    return textEditor;
}

void MainWindow::setCurrentIndex(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

void MainWindow::setCurrentWidget(QWidget *w){
    ui->stackedWidget->setCurrentWidget(w);
}

int MainWindow::getCurrentIndex()
{
    return ui->stackedWidget->currentIndex();
}

QWidget *MainWindow::getCurrentWidget()
{
    return ui->stackedWidget->currentWidget();
}

int MainWindow::indOf(QWidget *q)
{
    return ui->stackedWidget->indexOf(q);
}
*/

/*void MainWindow::userLogIn(Account *user){
    this->user=user;
    if(ui->stackedWidget->currentWidget()==login){
        showDocsBrowserPage();
    }
}
void MainWindow::userLogOut(){
    this->user=nullptr;
    if(ui->stackedWidget->currentWidget()==docsBrowser||ui->stackedWidget->currentWidget()==textEditor){
        showLoginPage();
    }
}*/
