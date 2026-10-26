#include "mydatabase.h"

#include <QDebug>
#include <QDir>
#include <QStandardPaths>

#include "globalutils.h"

extern bool isDBOpened;
extern bool databaseWorking;

// Fill the albumartist column of a library scanned before it existed
static void fillAlbumArtists()
{
    QStringList artists;
    QSqlQuery query = getQuery("select distinct artist from tracks");
    while (query.next())
        artists.append(query.value(0).toString());

    database.transaction();
    foreach (const QString &artist, artists)
    {
        // Stored escaped, see DataReader::reemplazar1()
        QString albumArtist = artist;
        albumArtist.replace("&lt;", "<").replace("&gt;", ">").replace("&quot;", "\"").replace("&apos;", "\'").replace("&amp;", "&");
        albumArtist = groupArtist(albumArtist);
        albumArtist.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;").replace("\"", "&quot;").replace("\'", "&apos;");

        QSqlQuery update(database);
        update.prepare("update tracks set albumartist=? where artist=?");
        update.addBindValue(albumArtist);
        update.addBindValue(artist);
        update.exec();
    }
    database.commit();
}

void openDatabase()
{
    if (database.isOpen())
        database.close();

    qDebug() << "DATABASES: " << database.connectionNames().join(" - ");

    if (!database.connectionNames().join("-").contains("flowplayer")) {
        database = QSqlDatabase::addDatabase("QSQLITE", "flowplayer");
        QString path(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/flowplayer.db");
        qDebug() << "Database: " << path;
        database.setDatabaseName(path);
    }
    QDir d;
    d.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));

    if (database.open()) {
        isDBOpened = true;
        executeQuery("alter table tracks add fav integer");
        if (executeQuery("alter table tracks add albumartist text"))
            fillAlbumArtists();
    } else {
        qWarning() << "cannot open database" << database.databaseName() << database.lastError().text();
    }
}

QSqlQuery getQuery(QString qr)
{
    if (database.open())
        isDBOpened = true;

    databaseWorking = true;
    QSqlQuery query(database);
    query.exec(qr);

    databaseWorking = false;
    return query;
}

bool executeQuery(QString qr)
{
    if (database.open())
        isDBOpened = true;

    databaseWorking = true;
    QSqlQuery *query = new QSqlQuery(database);
    bool res = query->exec(qr);
    delete query;

    databaseWorking = false;
    return res;
}

bool executeQueryCheckCount(QString qr)
{
    if (database.open())
        isDBOpened = true;

    databaseWorking = true;
    QSqlQuery *query = new QSqlQuery(database);
    query->exec(qr);
    bool res;
    if (query->next())
        res = true;
    else
        res = false;

    databaseWorking = false;
    return res;
    delete query;

}

