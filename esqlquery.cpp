#include "esqlquery.h"

#include <QSqlQuery>
#include <QDebug>

ESqlQuery::ESqlQuery()
{

}

void ESqlQuery::initTables() {
    this->exec("DROP TABLE items");
    this->exec("DROP TABLE inventory");
    bool items = query.exec("CREATE TABLE IF NOT EXISTS items"
                            " (id integer PRIMARY KEY NOT NULL, name VARCHAR(50), url VARCHAR(50));");
    if(items) {
        qDebug() << "ESqlQuery::initTables:: таблица items создана";
    }else {
        qDebug() << "ESqlQuery::initTables:: не удалось создать таблицу items";
    }
    bool inventory = query.exec("CREATE TABLE IF NOT EXISTS inventory"
                                " (id integer PRIMARY KEY NOT NULL, num_row INT UNIQUE, num_column INT UNIQUE, item INT, count INT,"
                                " FOREIGN KEY (item) REFERENCES items(id));");
    if(inventory) {
        qDebug() << "ESqlQuery::initTables:: таблица inventory создана";
    }else {
        qDebug() << "ESqlQuery::initTables:: не удалось создать таблицу inventory";
    }
}

void ESqlQuery::insertIntoItems(QString type, QString url) {
    this->prepare("INSERT INTO items (name, url)"
                  " VALUES (:name, :url)");
    this->bindValue(":name", QString(type));
    this->bindValue(":url", url);
    if(this->exec()) {
        qDebug() << "ESqlQuery::insertIntoItems:: объект " + type + " создан в таблице items";
    }else {
        qDebug() << "ESqlQuery::insertIntoItems:: не удалось создать объект " + type + " в таблице items";
    }
}
