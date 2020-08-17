#include "image_utils.h"

#include <QImage>
#include <QByteArray>
#include <QString>
#include <QBuffer>
#include <QDir>
#include <QStringList>
#include <QPixmap>
#include <QPainter>
#include <QIcon>
#include <QPainterPath>
#include <QFileInfo>

#include <vector>

#include "exceptions.h"
#include "utils.h"

using namespace se_exceptions;

QIcon image_utils::loadImage(const QString &path) {
  auto image = QIcon{path};

  if(image.isNull()) {
   throw ImageException{"Image not found!"};
  }

  return image;
}

QIcon image_utils::decodeImage(const QString &encoded) {
  auto pixmap = QPixmap();
  pixmap.loadFromData(QByteArray::fromBase64(encoded.toUtf8()));
  return QIcon(pixmap);
}

QString image_utils::encodeImage(const QIcon &icon) {
  QByteArray ba;
  QBuffer buffer(&ba);
  buffer.open(QIODevice::WriteOnly);
  icon.pixmap(300,300).toImage().save(&buffer, "PNG");

  return QString::fromUtf8(ba.toBase64());
}

std::vector<QIcon> image_utils::getAllProfilePics() {
  std::vector<QIcon> images;

  QDir directory(PROFILE_PICS_PATH);
  QStringList names = directory.entryList(QStringList() << "*.png" << "*.PNG",QDir::Files);
  foreach(QString filename, names) {
    images.push_back(loadImage(PROFILE_PICS_PATH + filename));
  }

  return images;
}

QIcon image_utils::loadRoundedImage(const QString &filename) {
  checkIcon(filename);

  const QPixmap orig = QPixmap(filename);

  if(orig.isNull()) {
   throw ImageException{"Could not load image!"};
  }

  // getting size if the original picture is not square
  int size = qMax(orig.width(), orig.height());
  // creating a new transparent pixmap with equal sides
  QPixmap rounded = QPixmap(size, size);
  rounded.fill(Qt::transparent);
  // creating circle clip area
  QPainterPath path;
  path.addEllipse(rounded.rect());
  QPainter painter(&rounded);
  painter.setClipPath(path);
  // filling rounded area if needed
  painter.fillRect(rounded.rect(), Qt::black);
  // getting offsets if the original picture is not square
  int x = qAbs(orig.width() - size) / 2;
  int y = qAbs(orig.height() - size) / 2;
  painter.drawPixmap(x, y, orig.width(), orig.height(), orig);

  return QIcon(rounded);
}

void image_utils::checkIcon(const QString &filename) {
  auto file = QFileInfo{filename};

  if(!file.exists()) {
    throw ImageException{"Image not found!"};

  }

  if(file.size() > MAX_SIZE) {
    throw ImageException{"Image is too big!"};
  }
}
