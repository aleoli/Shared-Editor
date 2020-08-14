#include "landing.h"
#include "../ui/ui_landing.h"

Landing::Landing(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Landing)
{
    ui->setupUi(this);
}

Landing::~Landing()
{
    delete ui;
}

void Landing::clear() {
  // TODO Cose da fare quando la finestra è portata in primo piano (es. svuotare i campi)
}
