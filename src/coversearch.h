#ifndef COVERSEARCH_H
#define COVERSEARCH_H

#include <QAbstractListModel>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBuffer>

#include "globalutils.h"
#include "loadwebimage.h"

class CoverSearch : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_ENUMS(ItemType)

public:
    enum CoverSearchRole
    {
        NameRole  = Qt::UserRole + 1 ,
        DescRole ,
        ArtRole
    };

    explicit CoverSearch(QObject * parent = 0);
    virtual ~CoverSearch();
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index , int role = Qt::DisplayRole) const;
    int count() const;
    QString path() const;

    QNetworkAccessManager* datos;
    QNetworkReply *reply;


signals:
    void countChanged();
    void searchFinished();
    void downloadFinished();
    void imageChanged(QString dartist, QString dalbum);

public slots:
    void clearData();
    void stopSearch();
    void searchCover(QString artist, QString album, QString data);
    void paintImg(QString image, int index);
    void saveImage(QString artist, QString album, QString imagepath);
    void saveArtistImage(QString artist, QString imagepath);

    void remove(const QString & file);

private slots:
    void downloaded(QNetworkReply *respuesta);


private:
    QHash<int, QByteArray> roleNames() const;
    void setRoleNames(const QHash<int, QByteArray>& roles);
    QHash<int, QByteArray> m_roles;

    QBuffer *buffer;
    QByteArray bytes;
    int Request;

    class CoverSearchPrivate;
    CoverSearchPrivate * const d;

};




#endif // COVERSEARCH_H
