#include "docsbrowser.h"
#include "../ui/ui_docsbrowser.h"

DocsBrowser::DocsBrowser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DocsBrowser)
{
    ui->setupUi(this);
}

DocsBrowser::~DocsBrowser()
{
    delete ui;
}
