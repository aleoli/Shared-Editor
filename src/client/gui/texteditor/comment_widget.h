#pragma once

#include <QWidget>
#include <QPushButton>

namespace Ui {
class Comment;
}

class CommentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommentWidget(QWidget *parent = nullptr);
    ~CommentWidget();

private:
    Ui::Comment *ui;
};
