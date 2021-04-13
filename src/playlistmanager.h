#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QQuickItem>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "mydatabase.h"

class PlaylistManager : public QQuickItem
{
    Q_OBJECT

public:
    PlaylistManager(QQuickItem *parent = 0);
    QList<QStringList> listado;

    QString xmlin(QString data);
    QString xmlout(QString data);

public slots:
    void loadList(QString list);
    void clearList(QString list);
    void addToList(QString artist, QString album, QString title, QString link, QString time);
    void addAlbumToList(QString list, QString artist, QString album, QString various, QString song);
    void copyListToQueue(QString source);
    void removeFromList(QString link);
    void saveList(QString list);
    void removeList(QString name);
    void renameList(QString oldname, QString newname);

    void loadPlaylists();
    void loadPlaylist(QString name);

    void loadArtist(QString artist);
    void loadAlbum(QString artist, QString album, QString various);


signals:
    void addPlaylist(QVariantMap list);
    void addItemToList(QString list, QVariantMap item);
    void addItemToListDone(QString list);
    void addItemToAlbum(QVariantMap item);
    void addItemToArtist(QVariantMap item);
    void albumLoaded(int totaltime);
    void artistLoaded(int totalsongs);


};

#endif // PLAYLISTMANAGER_H
