#include "registration.h"
#include "../ui/ui_registration.h"

Registration::Registration(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Registration)
{
    ui->setupUi(this);
}

Registration::~Registration()
{
    delete ui;
}
