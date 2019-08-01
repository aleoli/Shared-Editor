# cpp-persistence

## DB

Classe Singleton che deve essere instanziata all'inizio del main

```cpp
DB *db = DB::get();
(void) db;
```

Ha 3 metodi pubblici statici:

* `getAll<T>()`
* `getOne<T>(int id)`
* `get(QString filter)`

che ritornano rispettivamente `std::vector<T>` di tutti gli oggetti di tipo T, un oggetto di tipo T (o un'eccezione), `std::vector<T>` di tutti gli oggetti T che ripettano il filtro

L'oggetto T deve estendere pubblicamente la classe `persistence::Persistent`

Esempio:

```cpp
using persistence::Persistent;
using persistence::LazyList;

class Employee: public Persistent {
public:
    Employee(const Employee &e);
    Employee(Employee &&e);
    explicit Employee();
    explicit Employee(QSqlRecord r);

    virtual ~Employee();

    virtual Employee& operator=(const Employee& e);

    QString getName() const;
    void setName(QString name);
    int getSalary() const;
    void setSalary(int salary);
    std::vector<Project*> getProjects();

    virtual void save();
    virtual void save_record(QSqlRecord &r);
    virtual void remove();

    const static QString table_name;

private:
    QString name;
    int salary;
    LazyList<Project> projects;
};
```

per ottenere tutti gli impiegati

```cpp
std::vector<Employee> emp_list(DB::getAll<Employee>());

for(auto it=emp_list.begin(); it!=emp_list.end(); ++it) {
    std::cout << it->getName().toStdString() << it->getSalary() << std::endl;
}
```



Un oggetto `persistent` può contenere al suo interno altri oggetti `persistent` che vengono valutati in maniera Lazy



## Lazy

La variabile incapsulata all'interno viene caricata in maniera lazy rispetto al parent

Esempio:

```cpp
Project::Project(QSqlRecord r): Persistent(r) {
    this->name = r.value("name").toString();
    this->employee = Lazy<Employee>{r.value("employee").toInt()};
}
```

inserisco l'id dell'impiegato all'interno della variabile lazy che accederà al DB solo se utilizzo questa variabile

```cpp
// non fa accessi
int Project::getEmployeeId() const {
    return this->employee.getId();
}

// accede al DB
Employee Project::getEmployee() {
    return this->employee.getValue();
}
```



## LazyList

Funziona esattamente come `Lazy`, può essere costruita a partire da un vettore di id o da una tabella e un filtro

Esempio:

```cpp
Employee::Employee(QSqlRecord r): Persistent(r) {
    this->name = r.value("name").toString();
    this->salary = r.value("salary").toInt();
    this->projects = LazyList<Project>("project", "employee = "+QString::number(this->id));
}
```





Per dettagli guarda il progetto di esempio in example