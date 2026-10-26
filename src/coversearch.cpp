#include "coversearch.h"

//#include <QDeclarativeView>
#include <QDebug>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>

// Search words without Lucene syntax characters, e.g. "Artist - Album"
static QString searchWords(QString text)
{
    return text.replace(QRegExp("[-+&|!(){}\\[\\]^\"~*?:\\\\/]"), " ").simplified();
}

#define BING_ID "A16EECFD14108C2794E4BC29D4DE59C308685B4A"

struct CoverSearchItem
{
    QString name;
    QString desc;
    QString art;
};

class CoverSearch::CoverSearchPrivate
{
public:
    CoverSearchPrivate(CoverSearch * parent);
    ~CoverSearchPrivate();
    void populateItems();
    QList <CoverSearchItem * > items;
    CoverSearch * const q;
    QStringList foundedCovers;
    QString searchArt, searchAlb, searchData;
    QStringList searchIds;
    int searchTry, curDown, retries;

};

CoverSearch::CoverSearchPrivate::CoverSearchPrivate(CoverSearch * parent) : q(parent)
{
    q->datos = new QNetworkAccessManager(parent);
    //connect(q->datos, SIGNAL(finished(QNetworkReply*)), parent, SLOT(downloaded(QNetworkReply*)));

    //q->pepe = new WebThread();
    //connect (q->pepe, SIGNAL(imgLoaded(QString,int)), parent, SLOT(paintImg(QString,int)) );

    // Ensure that download destination exists.
    QDir d;
    d.mkpath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
}

CoverSearch::CoverSearchPrivate::~CoverSearchPrivate()
{
    while(!items.isEmpty())
    {
        delete items.takeFirst();
    }
}

void CoverSearch::CoverSearchPrivate::populateItems()
{
    foundedCovers.removeAll("");
    foundedCovers.removeDuplicates();

    int itemsCount = items.count();
    int entriesCount = foundedCovers.count();

    //qDebug() << "Items: " << itemsCount << " - Entries: " << entriesCount;

    if(entriesCount < itemsCount)
    {
        q->beginRemoveRows(QModelIndex() , entriesCount , itemsCount);

        for (int i = entriesCount ; i < itemsCount ; i++)
            delete items.takeLast();

        q->endRemoveRows();
    }
    else if (entriesCount > itemsCount)
    {
        q->beginInsertRows(QModelIndex() , itemsCount , entriesCount - 1);

        for(int i = itemsCount ; i < entriesCount ; i++)
            items.append(new CoverSearchItem);

        q->endInsertRows();
    }

    QListIterator<QString> entriesIterator (foundedCovers);
    QListIterator<CoverSearchItem *> itemsIterator(items);

    while(entriesIterator.hasNext() && itemsIterator.hasNext())
    {
        CoverSearchItem * item = itemsIterator.next();
        item->name = entriesIterator.next();
        item->desc = item->name;
        item->art = "loading"; //item->name;
    }

    emit(q->countChanged());

    emit(q->dataChanged(q->index(0) , q->index(entriesCount - 1)));

}

CoverSearch::CoverSearch(QObject * parent) :
        QAbstractListModel(parent) , d (new CoverSearchPrivate(this))
{
    // Roles
    QHash <int, QByteArray> roles;
    roles.insert(NameRole , "name");
    roles.insert(DescRole , "date");
    roles.insert(ArtRole , "art");
    d->q->setRoleNames(roles);

}

QHash<int, QByteArray> CoverSearch::roleNames() const {
    return m_roles;
}

void CoverSearch::setRoleNames(const QHash<int, QByteArray>& roles) {
    m_roles = roles;
}

CoverSearch::~CoverSearch()
{
    delete d;
}

int CoverSearch::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent)
    return d->items.size();
}

QVariant CoverSearch::data(const QModelIndex & index , int role) const
{
    if(index.row() < 0 or index.row() > count())
    {
        return QVariant();
    }

    CoverSearchItem * item = d->items.at(index.row());
    switch(role)
    {
    case NameRole:
        return item->name;
        break;
    case DescRole:
        return item->desc;
        break;
    case ArtRole:
        return item->art;
        break;
    default:
        return QVariant();
        break;
    }
}

int CoverSearch::count() const
{
    return rowCount();
}

void CoverSearch::remove(const QString &file)
{
    QString nf = file;
    if ( nf.startsWith("//") )
        nf.remove(0, 1);
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/flowplayer.conf", QSettings::NativeFormat);
    QStringList entries = settings.value("CoverSearch","").toStringList();
    QStringList newfiles;
    for (int i=0; i< entries.count(); ++i)
    {
        if ( entries.at(i) != nf )
            newfiles.append( entries.at(i) );
    }
    settings.setValue("CoverSearch", newfiles);
    settings.sync();
    d->populateItems();
}

void CoverSearch::searchCover(QString artist, QString album, QString data)
{
    d->searchArt = artist;
    d->searchAlb = album;
    d->searchData = data;
    d->searchTry = 1;
    d->retries = 0;

    disconnect(datos, 0, 0, 0);
    connect(datos, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded(QNetworkReply*)));

    // Artist photos: find the artist on MusicBrainz, then its Wikidata image
    if (album.isEmpty())
    {
        d->searchTry = 10;
        QString url = "https://musicbrainz.org/ws/2/artist/?fmt=json&limit=5&query=";
        url += QUrl::toPercentEncoding(searchWords(data));
        reply = datos->get(webRequest(QUrl::fromEncoded(url.toLatin1())));
        return;
    }

    QString url = "https://ws.audioscrobbler.com/2.0/?method=album.getinfo&api_key=7f338c7458e7d1a9a6204221ff904ba1";
    url += "&artist=" + QUrl::toPercentEncoding(d->searchArt) + "&album=" + QUrl::toPercentEncoding(d->searchAlb);
    reply = datos->get(QNetworkRequest(QUrl::fromEncoded(url.toLatin1())));

}

void CoverSearch::downloaded(QNetworkReply *respuesta)
{
    // MusicBrainz answers 503 when it is busy: ask again a bit later
    if (respuesta->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 503 && d->retries < 3)
    {
        ++d->retries;
        QUrl url = respuesta->request().url();
        QTimer::singleShot(3000 * d->retries, this, [this, url]() { reply = datos->get(webRequest(url)); });
        return;
    }

    if ( d->searchTry == 1 )
    {
        d->foundedCovers.clear();

        if (respuesta->error() != QNetworkReply::NoError)
        {
            qDebug() << "error: " << respuesta->error();
        }
        else
        {
            QString datos1 = respuesta->readAll();
            //qDebug() << datos1;
            QString tmp = datos1;
            int x = tmp.indexOf("<image size=\"mega\">");
            if (x >= 0)
            {
                tmp.remove(0,x+19);
                x = tmp.indexOf("<");
                tmp.remove(x,tmp.length()-x);
                tmp = tmp.trimmed();
                qDebug() << "Image founded... downloading... " << tmp;
                if (tmp.startsWith("http")) {
                    d->foundedCovers.append(tmp);
                }
            }
        }

        d->searchTry = 2;

        // Exact artist and album first, then whatever matches the words of
        // the search field (spelling differences, user edits)
        QString query = "(releasegroup:\"" + luceneEscape(d->searchAlb) + "\" AND artist:\""
                        + luceneEscape(d->searchArt) + "\")^3";
        if (!searchWords(d->searchData).isEmpty())
            query += " OR (" + searchWords(d->searchData) + ")";

        QString url = "https://musicbrainz.org/ws/2/release-group/?fmt=json&limit=10&query=";
        url += QUrl::toPercentEncoding(query);

        reply = datos->get(webRequest(QUrl::fromEncoded(url.toLatin1())));

    }
    else if ( d->searchTry == 2 )
    {
        if (respuesta->error() != QNetworkReply::NoError)
        {
            qDebug() << "error: " << respuesta->error();
        }
        else
        {
            QJsonDocument jsonResponse = QJsonDocument::fromJson(respuesta->readAll());

            foreach(const QJsonValue &group, jsonResponse.object().value("release-groups").toArray())
            {
                d->foundedCovers.append("https://coverartarchive.org/release-group/" + group.toObject().value("id").toString() + "/front-250");
            }

            if ( d->foundedCovers.count() > 0 )
            {
                d->populateItems();

                d->curDown = 0;
                d->searchTry = 3;
                QString url = d->foundedCovers.at(0);
                reply = datos->get(webRequest(QUrl(url)));

            }

        }
        emit searchFinished();
    }
    else if ( d->searchTry == 10 )
    {
        d->foundedCovers.clear();

        QStringList ids;
        if (respuesta->error() == QNetworkReply::NoError)
        {
            foreach (const QJsonValue &artist, QJsonDocument::fromJson(respuesta->readAll()).object().value("artists").toArray())
                ids.append(artist.toObject().value("id").toString());
        }

        if (ids.isEmpty())
        {
            emit searchFinished();
            return;
        }

        d->searchTry = 11;
        d->searchIds = ids;
        reply = datos->get(webRequest(wikidataPhotosUrl(ids)));
    }
    else if ( d->searchTry == 11 )
    {
        // Best matching artists first
        if (respuesta->error() == QNetworkReply::NoError)
        {
            QMap<QString, QString> photos = wikidataPhotos(respuesta->readAll());
            foreach (const QString &id, d->searchIds)
            {
                if (photos.contains(id))
                    d->foundedCovers.append(photos.value(id));
            }
        }

        if ( d->foundedCovers.count() > 0 )
        {
            d->populateItems();

            d->curDown = 0;
            d->searchTry = 3;
            reply = datos->get(webRequest(QUrl(d->foundedCovers.at(0))));
        }

        emit searchFinished();
    }
    else if ( d->searchTry == 3 )
    {
        QImage image;
        if (respuesta->error() == QNetworkReply::NoError)
            image = QImage::fromData(respuesta->readAll());
        QString path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/" + hash(d->foundedCovers.at(0)) + ".jpeg";

        if (!image.isNull() && image.save(path, "JPEG"))
        {
            d->items.at(d->curDown)->art = path;
            emit(d->q->dataChanged(d->q->index(d->curDown) , d->q->index(d->curDown)));
            d->curDown = d->curDown +1;
        }
        else
        {
            // Not every release group has cover art
            qDebug() << "Cover not available:" << d->foundedCovers.at(0) << respuesta->error();
            d->q->beginRemoveRows(QModelIndex(), d->curDown, d->curDown);
            delete d->items.takeAt(d->curDown);
            d->q->endRemoveRows();
            emit(d->q->countChanged());
        }

        if (d->foundedCovers.count()>0)
            d->foundedCovers.removeFirst();

        if (d->foundedCovers.count()>0) {
            d->searchTry = 3;
            QString url = d->foundedCovers.at(0);
            reply = datos->get(webRequest(QUrl(url)));
        } else {
            emit downloadFinished();
        }

    }


}

void CoverSearch::clearData()
{
    d->foundedCovers.clear();
    d->populateItems();
}

void CoverSearch::stopSearch()
{
    //if ( reply && reply->isRunning() )
    //    reply->abort();
    disconnect(datos, 0, 0, 0);
    clearData();
}

void CoverSearch::paintImg(QString image, int index)
{
    /*qDebug() << "LOADED " << index << " - " << image;

    if (d->items.count()>0) {
        d->items.at(index)->art = image;
        emit(d->q->dataChanged(d->q->index(index) , d->q->index(index)));
    }

    for (int i=0; i<d->items.count(); ++i)
    {
        if (d->items.at(i)->art=="") {
            pepe->downloadImageExtern(d->foundedCovers.at(i), i);
            break;
        }
    }*/
    //emit d->q->albumCoverChanged(image);
}

void CoverSearch::saveImage(QString artist, QString album, QString imagepath)
{
    QString th2 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/media-art/album-"+ doubleHash(artist, album) + ".jpeg";

    QImage image(imagepath);
    image.save(th2, "JPEG");

    emit imageChanged(artist, album);
}

void CoverSearch::saveArtistImage(QString artist, QString imagepath)
{
    QString th2 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/artist-" + hash(artist) + ".jpeg";

    QImage image(imagepath);
    image.save(th2, "JPEG");

    emit imageChanged(artist, "");
}
