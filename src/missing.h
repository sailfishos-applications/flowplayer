#ifndef MISSING_H
#define MISSING_H

#include <QAbstractListModel>
#include <QSettings>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBuffer>

#include "globalutils.h"
#include "mydatabase.h"
#include "loadwebimage.h"

class Missing : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadChanged)
    Q_PROPERTY(bool runthread READ runthread NOTIFY pepeChanged)

public:
    enum MissingRole
    {
        ArtistRole  = Qt::UserRole + 1,
        TitleRole, AcountRole, SongsRole, BandRole, CoverRole, HashRole, SelRole
    };

    explicit Missing(QObject * parent = 0);
    virtual ~Missing();
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index , int role = Qt::DisplayRole) const;
    int count() const;
    QString path() const;

    bool loaded() const { return m_loaded; }
    bool m_loaded;

    bool runthread() const { return runPepe; }
    bool runPepe;

    QList<QStringList> listado, lista;
    QStringList lista1, lista2;
    QStringList lista3, lista4, lista5;

    QString reemplazar1(QString data);
    QString reemplazar2(QString data);

    QString getThumbnail(QString data);

    WebThread *pepe;

    QList<QStringList> missingAlbums;


signals:
    void countChanged();
    void loadChanged();
    void pepeChanged();

    void albumCoverChanged(QString coverpath);


public slots:
    void startPepe();
    void paintImg(QString image, int index);
    void paintFinished();
    void reloadImage(int index);
    void loadData();
    void addData(QString artist, QString title, QString acount, QString band, QString songs);
    void removeData(QString url);
    void sortData();
    void clearList();
    void clearMusic();
    void selectItem(int index);
    void selectAllItems();
    void deselectAllItems();
    void addMissing(QString artist, QString album, int index);
    void clearMissing();
    void startDownload();
    void cancelAll();

private:
    QHash<int, QByteArray> roleNames() const;
    void setRoleNames(const QHash<int, QByteArray>& roles);
    QHash<int, QByteArray> m_roles;

    class MissingPrivate;
    MissingPrivate * const d;
    QSqlDatabase db;

};

#endif // MISSING_H
