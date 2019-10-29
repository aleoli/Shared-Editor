#pragma once

#include <vector>
#include <QColor>

//TODO migliorare la logica (ovviamente), fai qualche ricerca 

class ColorGenerator {
public:
  ColorGenerator();
  explicit ColorGenerator(int maxColors);

  QColor getColor();

private:
  int _maxColors;
  int _counter = 0;
  std::vector<QString> _colors{"red", "blue", "green", "yellow", "pink", "brown", "orange", "grey", "purple"};
};
