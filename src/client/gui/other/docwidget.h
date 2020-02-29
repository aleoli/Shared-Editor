#ifndef DOCWIDGET_H
#define DOCWIDGET_H

#include <QWidget>
class Document;

namespace Ui {
class DocWidget;
}

class DocWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DocWidget(QWidget *parent = nullptr,Document *d=nullptr);
    ~DocWidget();

private:
    Ui::DocWidget *ui;
    Document *doc;
};

#endif // DOCWIDGET_H
