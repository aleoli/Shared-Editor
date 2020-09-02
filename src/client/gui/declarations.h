#pragma once

#include "File.h"
#include "Symbol.h"
#include "Identifier.h"
#include "Paragraph.h"
#include "FSElement.h"
#include <QString>

Q_DECLARE_METATYPE(std::list<Symbol>)
Q_DECLARE_METATYPE(std::list<Paragraph>)
Q_DECLARE_METATYPE(std::list<Identifier>)
Q_DECLARE_METATYPE(std::vector<FSElement>)
Q_DECLARE_METATYPE(File)
Q_DECLARE_METATYPE(File::Comment)
Q_DECLARE_METATYPE(Symbol)
Q_DECLARE_METATYPE(SymbolId)
Q_DECLARE_METATYPE(FSElement)
Q_DECLARE_METATYPE(std::optional<QString>)
Q_DECLARE_METATYPE(std::optional<int>)
Q_DECLARE_METATYPE(FSElement::FileInfo)

void registerClasses() {
  qRegisterMetaType<std::list<Symbol>>();
  qRegisterMetaType<std::list<Paragraph>>();
  qRegisterMetaType<std::list<Identifier>>();
  qRegisterMetaType<std::vector<FSElement>>();
  qRegisterMetaType<File>();
  qRegisterMetaType<File::Comment>();
  qRegisterMetaType<Symbol>();
  qRegisterMetaType<SymbolId>();
  qRegisterMetaType<FSElement>();
  qRegisterMetaType<std::optional<QString>>();
  qRegisterMetaType<std::optional<int>>();
  qRegisterMetaType<std::list<std::pair<QString,int>>>();
  qRegisterMetaType<FSElement::FileInfo>();
}
