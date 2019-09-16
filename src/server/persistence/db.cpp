#include "db.h"

using persistence::DB;

std::shared_ptr<DB> DB::instance = nullptr;
