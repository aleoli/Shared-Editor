#include "landing.h"
#include "../ui/ui_landing.h"

Landing::Landing(QWidget *parent) :
    MainWindow(parent),
    ui(new Ui::Landing)
{
    ui->setupUi(this);
}

Landing::~Landing()
{
    delete ui;
}

void Landing::clear() {
}
