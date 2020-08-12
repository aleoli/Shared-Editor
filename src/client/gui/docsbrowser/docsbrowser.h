#pragma once

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
