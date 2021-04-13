#ifndef MyPlaylist_H
#define MyPlaylist_H

#include <QAbstractListModel>
#include <QSettings>
#include <QStringList>
#include <QSettings>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "mydatabase.h"

class MyPlaylist : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:

    enum MyPlaylistRole
    {
        ArtistRole  = Qt::UserRole + 1,
        AlbumRole,
        TitleRole,
        ImageRole,
        LinkRole,
        DurationRole
    };

    explicit MyPlaylist(QObject * parent = 0);
    virtual ~MyPlaylist();
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index , int role = Qt::DisplayRole) const;
    int count() const;
    QString path() const;

    //QSettings *sets;
    QList<QStringList> listado;

    QString getThumbnail(QString artist, QString album);
    QString clear(QString data);

    QString xmlin(QString data);
    QString xmlout(QString data);


signals:
    void countChanged();
    void loaded();


public slots:
    void loadList(QString list);


signals:
    void listChanged();
    void activeChanged();
    void unknownChanged();

private:
    QHash<int, QByteArray> roleNames() const;
    void setRoleNames(const QHash<int, QByteArray>& roles);
    QHash<int, QByteArray> m_roles;

    class MyPlaylistPrivate;
    MyPlaylistPrivate * const d;
    QSqlDatabase db;

};


#endif // MyPlaylist_H
