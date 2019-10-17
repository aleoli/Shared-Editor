//
// Created by Gianluca on 07/08/2019.
//

#ifndef PROVA_QTEXTEDIT_TEXTEDIT_H
#define PROVA_QTEXTEDIT_TEXTEDIT_H

//TODO: questa classe (INCOMPLETA!) è stata inserita per effettuare delle prove
// riguardo ad altre classi, pertanto non deve far parte del progetto finale
// però può essere presa come base di partenza per la TextEdit vera e propria che verrà
// implementata da erich e klajdi, per cui va guardata con attenzione e poi eventualmente
// corretta per essere inserita nel progetto.
// Ho lasciato anche qualche commento per spiegare il funzionamento di certe cose

#include <QMainWindow>
#include <QTextEdit>
#include <vector>
#include <QChar>
#include <QListWidget>

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
    void change(int pos, int removed, int added);
    void cursorChanged();

private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();

    void initDock();
    void addFakeUsers();

    QTextEdit *textEdit;
    QAction *actionTextBold;
    QListWidget *dock;
    std::vector<QChar> symbols;

    bool flagBold, flagCursor, flagChange;
};


#endif //PROVA_QTEXTEDIT_TEXTEDIT_H
