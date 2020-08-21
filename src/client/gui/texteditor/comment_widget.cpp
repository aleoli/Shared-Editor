#include "comment_widget.h"
#include "../ui/ui_texteditor_comment.h"

#include "utils.h"
#include "exceptions.h"

using namespace se_exceptions;

CommentWidget::CommentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Comment)
{
    ui->setupUi(this);
}

CommentWidget::~CommentWidget()
{
    delete ui;
}
