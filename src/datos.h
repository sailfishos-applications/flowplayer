#ifndef DATOS_H
#define DATOS_H

#include <QAbstractListModel>
#include <QSettings>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>

#include "globalutils.h"
#include "loadimage.h"
#include "mydatabase.h"

class Datos : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadChanged)
    //Q_PROPERTY(bool runthread READ runthread NOTIFY pepeChanged)

public:
    Q_INVOKABLE QString dataInfo();
    Q_INVOKABLE QString getArtistsCovers();
    Q_INVOKABLE QString getAlbumsCovers();

    enum DatosRole
    {
        ArtistRole  = Qt::UserRole + 1,
        TitleRole, AcountRole, SongsRole, BandRole, CoverRole, HashRole, SelRole,
        AlbumRole, DurationRole, UrlRole, FavRole
    };

    explicit Datos(QObject * parent = 0);
    virtual ~Datos();
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index , int role = Qt::DisplayRole) const;
    int count() const;
    QString path() const;

    bool loaded() const { return m_loaded; }
    bool m_loaded;

    //bool runthread() const { return runPepe; }
    //bool runPepe;

    QList<QStringList> listado;

    QString reemplazar1(QString data);
    QString reemplazar2(QString data);

    QString getThumbnail(QString data, int index);

    //Thread *pepe;


signals:
    void countChanged();
    void loadChanged();
    //void pepeChanged();


public slots:
    void startup();
    void paintImg(QString image, int index);
    void reloadImage(int index);
    void reloadAll();
    void loadArtists();
    void loadAlbums(QString order);
    void loadSongs(QString order);
    void addData(QString artist, QString title, QString acount, QString band, QString songs);
    void removeData(QString url);
    void clearList();
    void clearMusic();
    void selectItem(int index);
    void selectAllItems();
    void deselectAllItems();
    void addFilterToQueue();

    void reloadItem(QString albumpath);

    void setFilter(QString text);

    void setFav(QString filename, QString fav);

private:
    QHash<int, QByteArray> roleNames() const;
    void setRoleNames(const QHash<int, QByteArray>& roles);
    QHash<int, QByteArray> m_roles;

    class DatosPrivate;
    DatosPrivate * const d;
    QSqlDatabase db;

};

#endif // DATOS_H
