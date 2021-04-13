#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QQuickItem>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "mydatabase.h"

class Playlist : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString next READ next)
    Q_PROPERTY(QString prev READ prev)
    Q_PROPERTY(QString random READ random)
    Q_PROPERTY(QString unknown READ unknown NOTIFY unknownChanged)


public:
    Q_INVOKABLE int curIndex(QString song);
    //Q_INVOKABLE QString songAt(int index);
    Q_INVOKABLE QString currentSongArtist();
    Q_INVOKABLE QString currentSongAlbum();
    Q_INVOKABLE QString currentSongTitle();
    Q_INVOKABLE int currentSongDuration();
    Q_INVOKABLE QString nextSongArtist();
    Q_INVOKABLE QString nextSongAlbum();
    Q_INVOKABLE QString prevSongArtist();
    Q_INVOKABLE QString prevSongAlbum();
    Q_INVOKABLE int current();

    Playlist(QQuickItem *parent = 0);
    QList<QStringList> listado;

    bool isRandom;
    QString next();
    QString prev();
    QString random();
    QString active() const;
    QString unknown() const;

    QString xmlin(QString data);
    QString xmlout(QString data);


public slots:
    void loadList(QString list);
    void clearList(QString list);
    void addToList(QString list, QString artist, QString album, QString title, QString link, QString time);
    void addAlbumToList(QString artist, QString album, QString various);
    void removeFromList(QString link);
    void saveList(QString list);
    void setCurrent(int val);
    void removeList(QString name);
    void setRandom(bool val);

    void changeUnknown(bool active);
    void changeMode(QString mode);

signals:
    void listChanged();
    void activeChanged();
    void unknownChanged();

private:
    QSqlDatabase db;

};

#endif // PLAYLIST_H
