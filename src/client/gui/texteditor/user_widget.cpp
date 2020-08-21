#include "user_widget.h"
#include "../ui/ui_texteditor_connectedUser.h"

#include "utils.h"
#include "exceptions.h"

using namespace se_exceptions;

UserWidget::UserWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectedUser)
{
  ui->setupUi(this);
}

UserWidget::~UserWidget()
{
    delete ui;
}
