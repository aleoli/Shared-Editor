#ifndef DOCSBROWSER_H
#define DOCSBROWSER_H

#include <QWidget>
#include <list>
#include "document.h"

namespace Ui {
class DocsBrowser;
}

class DocsBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit DocsBrowser(QWidget *parent = nullptr);
    ~DocsBrowser();

private:
    Ui::DocsBrowser *ui;
    list<Document> *userDocs;
signals:
    void showLoginPage();
    void open_file();
};

#endif // DOCSBROWSER_H
