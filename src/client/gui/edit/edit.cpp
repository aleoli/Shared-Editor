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

void Edit::clear() {
  // TODO Cose da fare quando la finestra è portata in primo piano (es. svuotare i campi)
}
