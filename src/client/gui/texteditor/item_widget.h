#pragma once

#include <QListWidget>
#include <QListWidgetItem>

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
