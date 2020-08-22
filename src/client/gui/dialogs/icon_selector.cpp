#include "icon_selector.h"

#include <utility>
#include "../ui/ui_iconSelector.h"

#include <QFileDialog>

#include "image_utils.h"
#include "exceptions.h"
#include "utils.h"

using namespace se_exceptions;

IconSelector::IconSelector(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Icons),
  _icon(std::nullopt)
{
  ui->setupUi(this);

  _widgetChoose  = findChild<QPushButton *>("btn_delete");
  _widgetCancel  = findChild<QPushButton *>("btn_cancel");
  _iconDefault  = findChild<QPushButton *>("btn_default");
  _iconParrot = findChild<QPushButton *>("btn_parrot");
  _iconHappyman1  = findChild<QPushButton *>("btn_happyman1");
  _iconSoldier  = findChild<QPushButton *>("btn_soldier");
  _iconSpaceship1  = findChild<QPushButton *>("btn_spaceship1");
  _iconSpaceship2 = findChild<QPushButton *>("btn_spaceship2");
  _iconSpaceship3  = findChild<QPushButton *>("btn_spaceship3");
  _iconSpaceship4  = findChild<QPushButton *>("btn_spaceship4");
  _iconStar  = findChild<QPushButton *>("btn_star");
  _iconSunset = findChild<QPushButton *>("btn_sunset");

  if(!_widgetChoose || !_widgetCancel || !_iconDefault || !_iconParrot ||
     !_iconHappyman1 || !_iconSoldier || !_iconSpaceship1 || !_iconSpaceship2
      || !_iconSpaceship3 || !_iconSpaceship4 || !_iconStar || !_iconSunset ) {
        throw GuiException{"One or more widgets in IconSelector are null"};
  }

  connect(_widgetChoose, &QPushButton::clicked, this, &IconSelector::_choose);
  connect(_widgetCancel, &QPushButton::clicked, this, &IconSelector::_cancel);
  connect(_iconDefault, &QPushButton::clicked, this, &IconSelector::_default);
  connect(_iconParrot, &QPushButton::clicked, this, &IconSelector::_parrot);
  connect(_iconHappyman1, &QPushButton::clicked, this, &IconSelector::_happyman1);
  connect(_iconSoldier, &QPushButton::clicked, this, &IconSelector::_soldier);
  connect(_iconSpaceship1, &QPushButton::clicked, this, &IconSelector::_spaceship1);
  connect(_iconSpaceship2, &QPushButton::clicked, this, &IconSelector::_spaceship2);
  connect(_iconSpaceship3, &QPushButton::clicked, this, &IconSelector::_spaceship3);
  connect(_iconSpaceship4, &QPushButton::clicked, this, &IconSelector::_spaceship4);
  connect(_iconStar, &QPushButton::clicked, this, &IconSelector::_star);
  connect(_iconSunset, &QPushButton::clicked, this, &IconSelector::_sunset);
}

IconSelector::~IconSelector()
{
  delete ui;
}

std::optional<QIcon> IconSelector::show(QWidget *parent) {
  IconSelector dia(parent);

  if (dia.exec()) {
    if(dia._icon)
      return std::move(dia._icon);
    else
     throw GuiException{"IconSelector accepted but icon not set!"};
  }
  else {
    return std::nullopt;
  }
}

void IconSelector::loadPredefinedIcon(const QString &name) {
  try {
    _icon = std::optional<QIcon>(std::move(image_utils::loadImage(name)));
  }
  catch(...) {
    throw GuiException{"Failed to load predefined icon."};
  }
}

bool IconSelector::loadIconFromFile() {
  auto filename = QFileDialog::getOpenFileName(this,
    tr("Open Image"), PROFILE_PICS_PATH, tr("Image Files (*.png *.jpg *.bmp)"));

    try {
      _icon = image_utils::loadRoundedImage(filename);
      return true;
    }
    catch(...) {
      return false;
    }
}

void IconSelector::_choose(bool checked) {
  if(loadIconFromFile()) accept();
}

void IconSelector::_cancel(bool checked) {
  reject();
}

void IconSelector::_default(bool checked) {
  loadPredefinedIcon(":/res/profile-pics/default.png");
  accept();
}

void IconSelector::_parrot(bool checked) {
  loadPredefinedIcon(":/res/profile-pics/parrot.png");
  accept();
}

void IconSelector::_happyman1(bool checked) {
  loadPredefinedIcon(":/res/profile-pics/happyman1.png");
  accept();
}

void IconSelector::_soldier(bool checked) {
  loadPredefinedIcon(":/res/profile-pics/soldier.png");
  accept();
}

void IconSelector::_spaceship1(bool checked) {
  loadPredefinedIcon(":/res/profile-pics/spaceship1.png");
  accept();
}

void IconSelector::_spaceship2(bool checked) {
  loadPredefinedIcon(":/res/profile-pics/spaceship2.png");
  accept();
}

void IconSelector::_spaceship3(bool checked) {
  loadPredefinedIcon(":/res/profile-pics/spaceship3.png");
  accept();
}

void IconSelector::_spaceship4(bool checked) {
  loadPredefinedIcon(":/res/profile-pics/spaceship4.png");
  accept();
}

void IconSelector::_star(bool checked) {
  loadPredefinedIcon(":/res/profile-pics/star.png");
  accept();
}

void IconSelector::_sunset(bool checked) {
  loadPredefinedIcon(":/res/profile-pics/sunset.png");
  accept();
}
