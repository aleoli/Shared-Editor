#include "landing.h"
#include "ui_landing.h"

Landing::Landing(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Landing)
{
    ui->setupUi(this);

    _movie = std::make_shared<QMovie>(":/imgs/loading");
    _loading = findChild<QLabel *>("loading");
    _loading->setMovie(_movie.get());
    _movie->start();
}

Landing::~Landing()
{
    delete ui;
}
