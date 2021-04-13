#ifndef MUSICMODEL_H
#define MUSICMODEL_H

#include <QAbstractListModel>
#include <QSettings>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>

#include "mydatabase.h"

class MusicModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadChanged)
    Q_PROPERTY(int time READ time NOTIFY timeChanged)

public:
    enum MusicModelRole
    {
        TitleRole  = Qt::UserRole + 1,
        ArtistRole, DurationRole, TrackRole,
        UrlRole, AlbumRole, YearRole
    };

    explicit MusicModel(QObject * parent = 0);
    virtual ~MusicModel();
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index , int role = Qt::DisplayRole) const;
    int count() const;
    QString path() const;

    bool loaded() const { return m_loaded; }
    bool m_loaded;

    int time() const { return m_time; }
    int m_time;


    QList<QStringList> listado;

    QString reemplazar1(QString data);
    QString reemplazar2(QString data);


signals:
    void countChanged();
    void loadChanged();
    void timeChanged();


public slots:
    void loadData(QString artist, QString album, QString various);
    void clearList();
    void clearMusic();

private:
    QHash<int, QByteArray> roleNames() const;
    void setRoleNames(const QHash<int, QByteArray>& roles);
    QHash<int, QByteArray> m_roles;

    class MusicModelPrivate;
    MusicModelPrivate * const d;
    QSqlDatabase db;

};

#endif // MUSICMODEL_H
