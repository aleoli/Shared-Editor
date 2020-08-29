#pragma once

#include <QDialog>
#include <QIcon>
#include <QPushButton>

#include <optional>

namespace Ui {
class Icons;
}

class IconSelector : public QDialog
{
  Q_OBJECT

public:
  explicit IconSelector(QWidget *parent = nullptr);
  ~IconSelector();

  static std::optional<QIcon> show(QWidget *parent = nullptr);

private:
  void loadPredefinedIcon(const QString &name);
  bool loadIconFromFile();

  void _choose(bool checked);
  void _cancel(bool checked);
  void _default(bool checked);
  void _parrot(bool checked);
  void _happyman1(bool checked);
  void _soldier(bool checked);
  void _spaceship1(bool checked);
  void _spaceship2(bool checked);
  void _spaceship3(bool checked);
  void _spaceship4(bool checked);
  void _star(bool checked);
  void _sunset(bool checked);

  Ui::Icons *ui;

  QPushButton *_widgetChoose, *_widgetCancel, *_iconDefault, *_iconParrot,
    *_iconHappyman1, *_iconSoldier, *_iconSpaceship1, *_iconSpaceship2,
    *_iconSpaceship3, *_iconSpaceship4, *_iconStar, *_iconSunset;

  std::optional<QIcon> _icon;
};
