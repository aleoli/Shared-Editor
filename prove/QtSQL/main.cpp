#include <iostream>

#include <QtCore/QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QVariant>

int main() {
    std::cout << "Hello, World!" << std::endl;

    const QString DRIVER("QSQLITE");

    if(QSqlDatabase::isDriverAvailable(DRIVER)) {
        std::cout << "Disponibile" << std::endl << std::endl;
    } else {
        std::cout << "ERRORE: No Driver" << std::endl;
        exit(1);
    }

    // creo connessione
    QSqlDatabase db = QSqlDatabase::addDatabase(DRIVER);

    // db temporaneo in memoria, se metto un nome diverso mi crea un file
    db.setDatabaseName(":memory:");
    //db.setDatabaseName("prova");

    if(!db.open()) {
        std::cout << "ERRORE: " << db.lastError().text().toStdString() << std::endl;
        exit(1);
    }


    {
        // creo tabella
        QSqlQuery query("CREATE TABLE people (id INTEGER PRIMARY KEY, name TEXT)");

        if(!query.isActive()) {
            std::cout << "ERRORE: " << query.lastError().text().toStdString() << std::endl;
            exit(1);
        }
    }


    {
        // altro modo per eseguire una query
        QSqlQuery query;
        // NB: non mettendo l'id va in auto-increment
        if(!query.exec("INSERT INTO people(name) VALUES('Alessandro Olivero'),('Gianluca Scopelliti')")) {
            std::cout << "ERRORE: " << query.lastError().text().toStdString() << std::endl;
            exit(1);
        }
    }


    {
        QSqlQuery query;
        query.prepare("SELECT name FROM people WHERE id = ?");
        query.addBindValue(1);

        if(!query.exec()) {
            std::cout << "ERRORE: " << query.lastError().text().toStdString() << std::endl;
            exit(1);
        }

        if(query.first())
            std::cout << query.value("name").toString().toStdString() << std::endl << std::endl;
        else
            std::cout << "person not found" << std::endl << std::endl;
    }


    {
        // query multipla
        QSqlQuery query;
        query.prepare("SELECT name FROM people");

        if(!query.exec()) {
            std::cout << "ERRORE: " << query.lastError().text().toStdString() << std::endl;
            exit(1);
        }

        while(query.next()) {
            // access fields of current record
            std::cout << query.value("name").toString().toStdString() << std::endl;
        }

        std::cout << std::endl;
    }


    return 0;
}