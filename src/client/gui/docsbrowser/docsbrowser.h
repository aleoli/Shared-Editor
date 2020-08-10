#ifndef DOCSBROWSER_H
#define DOCSBROWSER_H

#include <QMainWindow>

namespace Ui {
class DocsBrowser;
}

class DocsBrowser : public QMainWindow
{
    Q_OBJECT

public:
    explicit DocsBrowser(QWidget *parent = nullptr);
    ~DocsBrowser();


private slots:

signals:

private:
    Ui::DocsBrowser *ui;
};

#endif
