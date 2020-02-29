#include "docwidget.h"
#include "ui_docwidget.h"
#include <QLabel>
class Document;

DocWidget::DocWidget(QWidget *parent,Document *d) :
    QWidget(parent),
    ui(new Ui::DocWidget)
{
    ui->setupUi(this);
    this->doc=d;
    //maybe set some icon
    //QLabel *name= new QLabel(doc->getName());
}

DocWidget::~DocWidget()
{
    delete ui;
}
