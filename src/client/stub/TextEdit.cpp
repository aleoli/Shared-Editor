//
// Created by Gianluca on 07/08/2019.
//
#include "TextEdit.h"

#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <iostream>
#include <QTextDocument>
#include <QTextBlock>
#include <QFont>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListView>
#include <QListWidget>
#include <QListWidgetItem>

#include "Symbol.h"
#include "utils.h"

TextEdit::TextEdit(QWidget *parent)
        : QMainWindow(parent)
{
  // QTextEdit sarà il nostro widget principale, contiene un box in cui inserire testo.
  // Inoltre avremo un menu, in cui ci saranno tutte le varie opzioni, e una toolbar che ne avrà un set
  _textEdit = new QTextEdit(this);
  setCentralWidget(_textEdit);

  // inizializzo tutte le varie azioni. Un'azione può essere "Nuovo File", "Copia", "Incolla", "Grassetto", ...
  // per ogni azione va associato uno slot (funzione che esegue qualcosa quando si vuole eseguire l'azione
  // (esempio quando clicco su "Grassetto" (azione) voglio che il testo diventi appunto in grassetto (slot)
  setupFileActions();
  setupEditActions();
  setupTextActions();

  // signal/slot aggiunta / rimozione / modifica caratteri
  QObject::connect(_textEdit->document(), &QTextDocument::contentsChange, this, &TextEdit::change);
  QObject::connect(_textEdit, &QTextEdit::cursorPositionChanged, this, &TextEdit::cursorChanged);

  _flagBold = _flagCursor = _flagChange = false;

  // _dock right
  initDock();
  //addFakeUsers();
}

void TextEdit::setFile(const File &f, int charId) {
  _file = f;
  _charId = charId;

  //TODO inizializza editor visivo con i simboli del file
}

void TextEdit::closeEvent(QCloseEvent *event) {
  debug("Chiusura editor");

  emit closeFileRequest();
  event->accept();
}

void TextEdit::initDock() {
  auto *dockWidget = new QDockWidget(tr("Utenti connessi:"), this);
  dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
  addDockWidget(Qt::RightDockWidgetArea, dockWidget);

  _dock = new QListWidget(this);
  dockWidget->setWidget(_dock);
}

void TextEdit::addFakeUsers() {
  auto bob = new QListWidgetItem("bob", _dock);
  auto ale = new QListWidgetItem("ale", _dock);

  bob->setFlags(Qt::NoItemFlags);
  bob->setForeground(Qt::red);

  ale->setFlags(Qt::NoItemFlags);
  ale->setForeground(Qt::blue);
}

void TextEdit::setupFileActions() {
  // Azioni per i file (Nuovo file, salva...)

  // aggiunge toolbar, che sarebbe la barra in alto
  QToolBar *tb = addToolBar(tr("File Actions"));

  // aggiunge menu File
  QMenu *menu = menuBar()->addMenu(tr("&File"));

  const QIcon addLetterIcon(":/buttons/letter.png");
  QAction *add = menu->addAction(addLetterIcon,  tr("&Add"), this, &TextEdit::addLetter);
  tb->addAction(add);

  const QIcon getInfoIcon(":/buttons/info.png");
  QAction *info = menu->addAction(getInfoIcon,  tr("&Add"), this, &TextEdit::printTextFile);
  tb->addAction(info);
}

void TextEdit::setupEditActions() {
  // Azioni per la modifica (Taglia, copia, incolla, undo, redo...)

  // aggiungo toolbar per le impostazioni di modifica (copia/incolla...) e anche una voce nel menu
  QToolBar *tb = addToolBar(tr("Edit Actions"));
  QMenu *menu = menuBar()->addMenu(tr("&Edit"));

  // AZIONE COPIA
  const QIcon copyIcon(":/buttons/copy.png");

  // in questo caso, QTextEdit ha già uno slot che fa la copia del testo in automatico, non è da implementare
  QAction *actionCopy = menu->addAction(copyIcon, tr("&Copy"), _textEdit, &QTextEdit::copy);
  actionCopy->setPriority(QAction::LowPriority);

  //imposto la shortcut da usare (che sarebbe CTRL+C)
  actionCopy->setShortcut(QKeySequence::Copy);
  tb->addAction(actionCopy);

  // AZIONE INCOLLA
  const QIcon pasteIcon(":/buttons/paste.png");

  // in questo caso, QTextEdit ha già uno slot che fa l'incolla del testo in automatico, non è da implementare
  QAction *actionPaste = menu->addAction(pasteIcon, tr("&Paste"), _textEdit, &QTextEdit::paste);
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
  const QIcon boldIcon(":/buttons/bold.png");
  _actionTextBold = menu->addAction(boldIcon, tr("&Bold"), this, &TextEdit::textBold);
  // shortcut: CTRL+B
  _actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
  _actionTextBold->setPriority(QAction::LowPriority);

  //questo è solo per dare il grassetto alla voce nel menu file.. non mette in grassetto il testo ovviamente
  QFont bold;
  bold.setBold(true);
  _actionTextBold->setFont(bold);

  tb->addAction(_actionTextBold);
  _actionTextBold->setCheckable(true);
}

void TextEdit::textBold() {
  if(_flagBold) {
    _flagBold = false;
    return;
  }

  debug("Premuto tasto grassetto");

  // setto flag
  _flagChange = true;

  // setto il formato, se l'icona bold è checkata allora devo mettere in grassetto, altrimenti normale
  QTextCharFormat fmt;
  fmt.setFontWeight(_actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);

  //TODO rivedere logica
  QTextCursor cursor = _textEdit->textCursor();

  if (!cursor.hasSelection()) {
      cursor.select(QTextCursor::WordUnderCursor);
      cursor.mergeCharFormat(fmt);
  }

  //setto in grassetto
  _textEdit->mergeCurrentCharFormat(fmt);
}

void TextEdit::addLetter() {
  Symbol sym(SymbolId(), 'a');
  sym.setBold(true);
  sym.setSize(20);
  sym.setUnderline(true);
  sym.setItalic(true);
  sym.setFamily("Helvetica");
  sym.setColor("red");
  sym.setBackgroundColor("#6600ff00");

  _textEdit->textCursor().insertText(sym.getChar(), sym.getFormat());
}

void TextEdit::printTextFile() {
  info(QString::fromStdString(_file.text()));
  debug(QString::fromStdString(_file.to_string()));
}

void TextEdit::change(int pos, int removed, int added) {
  if(_flagChange) {
    _flagChange = false;
    return;
  }

  //TODO se removed == added -> UPDATE
  //TODO si potrebbe fare anche una cosa più elaborata,
  //    tipo 2 rem e 3 added significa 2 update e 1 added, ma non so se vale la pena

  debug("contentsChange: rimossi " + QString::number(removed) + " aggiunti " + QString::number(added));

  // verifico quali caratteri sono stati rimossi (accedendo al vettore di simboli)
  for(int i=0; i<removed; i++) {

    // individuo simbolo e lo rimuovo dal vettore
    _file.localDelete(pos);

    //TODO mandare messaggio al server
  }

  // verifico quali caratteri sono stati aggiunti (direttamente dall'editor)
  QTextCursor cursor(_textEdit->document());
  cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, pos);

  for(int i=0; i<added; i++) {

    auto chr = _textEdit->document()->characterAt(pos + i);
    auto fmt = cursor.charFormat();

    //creo symbol e lo aggiungo al file
    //TODO userId / clientId ???
    Symbol s{{0, _charId++}, chr, fmt};
    _file.localInsert(s, pos+i);

    //TODO mandare messaggio al server

    cursor.movePosition(QTextCursor::NextCharacter);
  }

  _flagCursor = true;
}

void TextEdit::cursorChanged() {
  if(_flagCursor) {
    _flagCursor = false;
    return;
  }

  auto font = _textEdit->currentFont();
  if(font.bold() != _actionTextBold->isChecked()) {
    _flagBold = true;
    _actionTextBold->activate(QAction::Trigger);
  }
}
