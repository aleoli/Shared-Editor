#include "docsbrowser.h"
#include "ui_docsbrowser.h"
#include "document.h"
#include <iostream>

DocsBrowser::DocsBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DocsBrowser)
{
    ui->setupUi(this);
    //loadDocs(user);
}

DocsBrowser::~DocsBrowser()
{
    delete ui;
}

void DocsBrowser::showDocsAvailable(){
    //for(Document d : *userDocs){
        //DocWidget *dw = new DocWidget(d);
        //appendDocWidget(dw);
    //}
}
