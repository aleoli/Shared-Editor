#include "edit.h"
#include "../ui/ui_edit.h"

Edit::Edit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Edit)
{
    ui->setupUi(this);
}

Edit::~Edit()
{
    delete ui;
}
