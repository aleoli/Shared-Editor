#ifndef LAZY_H
#define LAZY_H

#include "persistence.h"

#include "db.h"

using persistence::DB;

template <typename T>
class persistence::Lazy {
public:
    Lazy(int id=0): id(id), value(nullptr), has_value(false) {}

    Lazy(const Lazy& e) {
        if(this != &e) {
            if(this->value) {
              delete this->value;
              this->value = nullptr;
            }
            if(e.value) {
                this->value = new T{e.value};
            } else {
                this->value = nullptr;
            }
            this->id = e.id;
            this->has_value = e.has_value;
        }
    }

    Lazy(Lazy && e) {
      if(this != &e) {
        if(this->value) {
          delete this->value;
        }
        this->value = e.value;
        e.value = nullptr;
        this->id = e.id;
        this->has_value = e.has_value;
        e.has_value = false;
      }
    }

    ~Lazy() {
        if(this->value)
            delete this->value;
    }

    Lazy& operator =(const Lazy& e) {
        if(this != &e) {
            if(this->value) {
              delete this->value;
              this->value = nullptr;
            }
            if(e.value) {
                this->value = new T{*e.value};
            } else {
                this->value = nullptr;
            }
            this->id = e.id;
            this->has_value = e.has_value;
        }
        return *this;
    }

    Lazy& operator =(Lazy && e) {
      if(this != &e) {
        if(this->value) {
          delete this->value;
        }
        this->value = e.value;
        e.value = nullptr;
        this->id = e.id;
        this->has_value = e.has_value;
        e.has_value = false;
      }
      return *this;
    }

    T operator ->() {
        if(!this->has_value) {
            this->value = new T{DB::getOne<T>(this->id)};
            this->has_value = true;
        }
        return *(this->value);
    }

    T getValue() {
        return this->operator->();
    }

    int getId() const {
        return this->id;
    }

		void clear() {
				if(this->value)
						delete this->value;
            this->value = nullptr;
				this->value = nullptr;
				this->has_value = false;
		}

private:
    int id;
    T* value;
    bool has_value;
};

#endif // LAZY_H
