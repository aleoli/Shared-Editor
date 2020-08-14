#include "texteditor.h"
#include "../ui/ui_texteditor.h"

TextEditor::TextEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TextEditor)
{
    ui->setupUi(this);
}

TextEditor::~TextEditor()
{
    delete ui;
}

void TextEditor::clear() {
  // TODO Cose da fare quando la finestra è portata in primo piano (es. svuotare i campi)
}
