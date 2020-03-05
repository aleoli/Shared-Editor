#ifndef DOCSBROWSER_H
#define DOCSBROWSER_H

#include <QWidget>
#include <list>
#include <QString>

class Manager;
//class DocWidget;
//class Document;

namespace Ui {
class DocsBrowser;
}

class DocsBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit DocsBrowser(QWidget *parent = nullptr);
    ~DocsBrowser();
    void showDocsAvailable();
    void setManagerClass(Manager *m){this->m=m;}


private:
    Ui::DocsBrowser *ui;
   //QList<Document> *userDocs;
    Manager *m;
    
    
//public slots:
    
    
signals:
    void showLoginPage();
    void open_file();
    void newFileQuery(QString);
    void getFileQuery(int);
    void newFolderQuery(QString);
    void getFolderQuery(int);

};

#endif // DOCSBROWSER_H
