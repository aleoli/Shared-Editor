#include "color_generator.h"

#include <QString>

#include "exceptions.h"
#include "utils.h"

using namespace se_exceptions;

ColorGenerator::ColorGenerator() : _maxColors(256) {}

ColorGenerator::ColorGenerator(int maxColors) : _maxColors(maxColors) {}

QColor ColorGenerator::getColor() {
  if(_counter >= _colors.size()) {
    throw GuiException{"Out of colors!"};
  }

  auto color = _colors.at(_counter++);

  return QColor(color);
}

void ColorGenerator::reset() {
  _counter = 0;
}
