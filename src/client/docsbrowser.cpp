#include "docsbrowser.h"
#include "ui_docsbrowser.h"

DocsBrowser::DocsBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DocsBrowser)
{
    ui->setupUi(this);
}

DocsBrowser::~DocsBrowser()
{
    delete ui;
}
