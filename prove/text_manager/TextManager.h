//
// Created by aleoli on 07/08/19.
//

#ifndef TEXT_MANAGER_TEXTMANAGER_H
#define TEXT_MANAGER_TEXTMANAGER_H


#include <QObject>
#include <QString>

class TextManager: public QObject {
    Q_OBJECT
public:
    TextManager(QObject *parent);

    QString loadFromServer();
    void setPos(int pos);

public slots:
    void addStr(QString str);
    void addChr(QChar chr);

signals:
    void str_loaded(QString str, int pos);

private:
    int pos;
    QString str;
    std::vector<QString> html_tags;
};


#endif //TEXT_MANAGER_TEXTMANAGER_H
