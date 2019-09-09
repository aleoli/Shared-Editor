//
// Created by Gianluca on 07/08/2019.
//

#ifndef PROVA_QTEXTEDIT_TEXTEDIT_H
#define PROVA_QTEXTEDIT_TEXTEDIT_H

#include <QMainWindow>
#include <QTextEdit>

class TextEdit : public QMainWindow {
    Q_OBJECT
public:
    TextEdit(QWidget *parent = 0);

public slots:
    void fileNew();

protected:

private slots:
    void textBold();
    void addLetter();
    void getCharInfo();

private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();

    QTextEdit *textEdit;
    QAction *actionTextBold;
};


#endif //PROVA_QTEXTEDIT_TEXTEDIT_H
