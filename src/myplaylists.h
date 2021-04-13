#ifndef MyPlaylistsS_H
#define MyPlaylistsS_H

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

class MyPlaylists : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum MyPlaylistsRole
    {
        NameRole  = Qt::UserRole + 1,
        CountRole
    };

    explicit MyPlaylists(QObject * parent = 0);
    virtual ~MyPlaylists();
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index , int role = Qt::DisplayRole) const;
    int count() const;
    QString path() const;

    QString xmlin(QString data);
    QString xmlout(QString data);

signals:
    void countChanged();
    void pathChanged();


public slots:
    void loadMyPlaylists();

private:
    QHash<int, QByteArray> roleNames() const;
    void setRoleNames(const QHash<int, QByteArray>& roles);
    QHash<int, QByteArray> m_roles;

    class MyPlaylistsPrivate;
    MyPlaylistsPrivate * const d;
    QSqlDatabase db;

};


#endif // MyPlaylistsS_H
