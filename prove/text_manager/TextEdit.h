//
// Created by Gianluca on 07/08/2019.
//

#ifndef PROVA_QTEXTEDIT_TEXTEDIT_H
#define PROVA_QTEXTEDIT_TEXTEDIT_H

#include <QMainWindow>
#include <QTextEdit>

#include "TextManager.h"

class TextEdit : public QMainWindow {
Q_OBJECT
public:
    TextEdit(QWidget *parent = 0);

    void setRemoteInsert(bool v);

public slots:
    void fileNew();
    void addHtml(QString str, int pos);

protected:

private slots:
    void textBold();
    void ciao();
    void addChar();
    void positionChanged();

private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();

    QTextEdit *textEdit;
    QAction *actionTextBold;
    TextManager *txtManager;

    bool remote_insert;
};


#endif //PROVA_QTEXTEDIT_TEXTEDIT_H