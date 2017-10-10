#ifndef ESQLQUERY_H
#define ESQLQUERY_H

#include <QSqlQuery>

class ESqlQuery : public QSqlQuery
{
public:
    ESqlQuery();
    void initTables();
    void insertIntoItems(QString type, QString url);
    void removeFromInventory(int itemRow, int itemColumn);
    void updateInInventory(int itemRow, int itemColumn);
protected:
    QSqlQuery query;
};

#endif // ESQLQUERY_H
