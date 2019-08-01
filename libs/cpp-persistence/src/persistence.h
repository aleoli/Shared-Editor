#ifndef PERSISTENCE_H
#define PERSISTENCE_H

namespace persistence {

class DB;
template <typename T> class Lazy;
template <typename T> class LazyList;
class Persistent;

class SQLException;
class SQLInsertException;
class SQLNoElementException;
class SQLNoElementSelectException;
class SQLNoElementDeleteException;
class SQLNoElementUpdateException;

}

#endif // PERSISTENCE_H
