#pragma once

#include "File.h"
#include "Symbol.h"
#include "SymbolId.h"
#include "FSElement.h"
#include <QString>

Q_DECLARE_METATYPE(std::vector<Symbol>)
Q_DECLARE_METATYPE(std::vector<SymbolId>)
Q_DECLARE_METATYPE(std::vector<FSElement>)
Q_DECLARE_METATYPE(File)
Q_DECLARE_METATYPE(File::Comment)
Q_DECLARE_METATYPE(Symbol)
Q_DECLARE_METATYPE(SymbolId)
Q_DECLARE_METATYPE(FSElement)
Q_DECLARE_METATYPE(std::optional<QString>)
Q_DECLARE_METATYPE(std::optional<int>)

void registerClasses() {
  qRegisterMetaType<std::vector<Symbol> >();
  qRegisterMetaType<std::vector<SymbolId> >();
  qRegisterMetaType<std::vector<FSElement> >();
  qRegisterMetaType<File>();
  qRegisterMetaType<File::Comment>();
  qRegisterMetaType<Symbol>();
  qRegisterMetaType<SymbolId>();
  qRegisterMetaType<FSElement>();
  qRegisterMetaType<std::optional<QString>>();
  qRegisterMetaType<std::optional<int>>();
}
