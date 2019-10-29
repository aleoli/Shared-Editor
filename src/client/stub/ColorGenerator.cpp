#include "ColorGenerator.h"
#include "exceptions.h"
#include "utils.h"
#include <QString>

using namespace se_exceptions;

ColorGenerator::ColorGenerator() : _maxColors(256) {}

ColorGenerator::ColorGenerator(int maxColors) : _maxColors(maxColors) {}

QColor ColorGenerator::getColor() {
  if(_counter >= _colors.size()) {
    throw SE_Exception{"Finiti i colori"};
  }

  auto color = _colors.at(_counter++);

  return QColor(color);
}
