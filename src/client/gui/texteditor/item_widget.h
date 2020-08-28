#pragma once

#include <QListWidget>
#include <QListWidgetItem>

#define MAX_SIZE_NAME 10

class ItemWidget : public QWidget {
  Q_OBJECT

public:
  ItemWidget();

  void insert(int pos, QListWidget *list);
  void add(QListWidget *list);
  void remove(QListWidget *list);

  QListWidgetItem *getItem() const;

protected:
  QListWidgetItem *_item;
};
