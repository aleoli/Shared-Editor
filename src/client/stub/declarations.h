#pragma once

Q_DECLARE_METATYPE(std::vector<Symbol>)
Q_DECLARE_METATYPE(std::vector<SymbolId>)
Q_DECLARE_METATYPE(std::vector<FSElement>)
Q_DECLARE_METATYPE(File)
Q_DECLARE_METATYPE(Symbol)
Q_DECLARE_METATYPE(SymbolId)
Q_DECLARE_METATYPE(FSElement)

void registerClasses() {
  qRegisterMetaType<std::vector<Symbol> >();
  qRegisterMetaType<std::vector<SymbolId> >();
  qRegisterMetaType<std::vector<FSElement> >();
  qRegisterMetaType<File>();
  qRegisterMetaType<Symbol>();
  qRegisterMetaType<SymbolId>();
  qRegisterMetaType<FSElement>();
}
