#ifndef MYDATABASE_H
#define MYDATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QVariant>

static QSqlDatabase database;


void openDatabase();
bool executeQuery(QString qr);
bool executeQueryCheckCount(QString qr);
QSqlQuery getQuery(QString qr);


#endif // MYDATABASE_H
