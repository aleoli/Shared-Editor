//
// Created by Gianluca on 07/08/2019.
//
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <iostream>

#include "TextEdit.h"

TextEdit::TextEdit(QWidget *parent)
        : QMainWindow(parent)
{
    // QTextEdit sarà il nostro widget principale, contiene un box in cui inserire testo.
    // Inoltre avremo un menu, in cui ci saranno tutte le varie opzioni, e una toolbar che ne avrà un set
    textEdit = new QTextEdit(this);
    setCentralWidget(textEdit);

    // inizializzo tutte le varie azioni. Un'azione può essere "Nuovo File", "Copia", "Incolla", "Grassetto", ...
    // per ogni azione va associato uno slot (funzione che esegue qualcosa quando si vuole eseguire l'azione
    // (esempio quando clicco su "Grassetto" (azione) voglio che il testo diventi appunto in grassetto (slot)
    setupFileActions();
    setupEditActions();
    setupTextActions();

}

void TextEdit::setupFileActions() {
    // Azioni per i file (Nuovo file, salva...)

    // aggiunge toolbar, che sarebbe la barra in alto
    QToolBar *tb = addToolBar(tr("File Actions"));

    // aggiunge menu File
    QMenu *menu = menuBar()->addMenu(tr("&File"));

    // AZIONE NUOVO FILE
    // aggiungo una voce "Nuovo file", specificando l'icona per la toolbar, e aggiungo la voce sia nel menu che nella toolbar
    // inoltre, alla voce è collegato uno slot TextEdit::fileNew, che esegue azioni quando viene cliccata la voce
    const QIcon newIcon("../imgs/new.png");
    QAction *a = menu->addAction(newIcon,  tr("&New"), this, &TextEdit::fileNew);
    tb->addAction(a);


}

void TextEdit::setupEditActions() {
    // Azioni per la modifica (Taglia, copia, incolla, undo, redo...)

    // aggiungo toolbar per le impostazioni di modifica (copia/incolla...) e anche una voce nel menu
    QToolBar *tb = addToolBar(tr("Edit Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&Edit"));

    // AZIONE COPIA
    const QIcon copyIcon("../imgs/copy.png");

    // in questo caso, QTextEdit ha già uno slot che fa la copia del testo in automatico, non è da implementare
    QAction *actionCopy = menu->addAction(copyIcon, tr("&Copy"), textEdit, &QTextEdit::copy);
    actionCopy->setPriority(QAction::LowPriority);

    //imposto la shortcut da usare (che sarebbe CTRL+C)
    actionCopy->setShortcut(QKeySequence::Copy);
    tb->addAction(actionCopy);

    // AZIONE INCOLLA
    const QIcon pasteIcon("../imgs/paste.png");

    // in questo caso, QTextEdit ha già uno slot che fa l'incolla del testo in automatico, non è da implementare
    QAction *actionPaste = menu->addAction(pasteIcon, tr("&Paste"), textEdit, &QTextEdit::paste);
    actionPaste->setPriority(QAction::LowPriority);

    //imposto la shortcut da usare (che sarebbe CTRL+V)
    actionPaste->setShortcut(QKeySequence::Paste);
    tb->addAction(actionPaste);
}

void TextEdit::setupTextActions() {
    // Azioni per il testo (grassetto, corsivo, allineamento...)

    // aggiungo toolbar per le impostazioni del testo (grassetto, corsivo...)
    QToolBar *tb = addToolBar(tr("Format Actions"));
    QMenu *menu = menuBar()->addMenu(tr("F&ormat"));

    // AZIONE GRASSETTO
    // aggiungo un'azione per il grassetto, e ci associo uno slot (implementato da me)
    const QIcon boldIcon("../imgs/bold.png");
    actionTextBold = menu->addAction(boldIcon, tr("&Bold"), this, &TextEdit::textBold);
    // shortcut: CTRL+B
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);

    //questo è solo per dare il grassetto alla voce nel menu file.. non mette in grassetto il testo ovviamente
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);

    tb->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

}

void TextEdit::fileNew() {
    std::cout << "ciaoo" << std::endl;
}

void TextEdit::textBold() {
    // setto il formato, se l'icona bold è checkata allora devo mettere in grassetto, altrimenti normale
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);

    // dove metto in grassetto? se sto selezionando un testo, allora in tutto quel testo,
    // altrimenti setto in grassetto la parola in cui si trova il mio cursore in questo momento
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);

    //setto in grassetto
    cursor.mergeCharFormat(fmt);
    textEdit->mergeCurrentCharFormat(fmt);
}