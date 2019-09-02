#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#define IN_MEMORY 0

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
