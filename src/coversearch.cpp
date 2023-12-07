#include "coversearch.h"

//#include <QDeclarativeView>
#include <QDebug>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

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
    int searchTry, curDown;

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
    QSettings settings("cepiperez", "flowplayer");
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

    disconnect(datos, 0, 0, 0);
    connect(datos, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded(QNetworkReply*)));
    QString url = "http://ws.audioscrobbler.com/2.0/?method=album.getinfo&api_key=7f338c7458e7d1a9a6204221ff904ba1";
    reply = datos->get(QNetworkRequest(QUrl(url+"&artist="+d->searchArt+"&album="+d->searchAlb)));

}

void CoverSearch::downloaded(QNetworkReply *respuesta)
{
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
            tmp.remove(0,x+19);
            x = tmp.indexOf("<");
            tmp.remove(x,tmp.length()-x);
            tmp = tmp.trimmed();
            qDebug() << "Image founded... downloading... " << tmp;
            if (tmp!="") {
                d->foundedCovers.append(tmp);
            }
        }

        d->searchTry = 2;
        //QString url = "http://api.bing.net/xml.aspx?Appid=8818f55e-2fe5-4ce3-a617-0b8ba8419f65&query=" +
        //        d->searchData + "&sources=image"; //&Image.Filters=Size:medium";

        QString url = "https://www.googleapis.com/customsearch/v1?cx=017908193050600132175%3Ayspmyfc3vkg&";
        url += "q=" + d->searchData + "&searchType=image&imgSize=large&start=1";
        url += "&fields=items%2Flink%2Curl&key=AIzaSyClwjXKlbh7fQnrNajj3x1YEkxfHSqQqsE";
        //url += "&artist="+QUrl::toPercentEncoding(d->searchArt)+"&album="+QUrl::toPercentEncoding(d->searchAlb)+"&search=Search";

        reply = datos->get(QNetworkRequest(QUrl(url)));

    }
    else if ( d->searchTry == 2 )
    {
        if (respuesta->error() != QNetworkReply::NoError)
        {
            qDebug() << "error: " << respuesta->error();
        }
        else
        {
            QString datoshtml = respuesta->readAll();
            //qDebug() << datoshtml;
            /*QStringList rawmatches;
            QString tmp;
            QRegExp rx;
            int pos = 0;
            int count = 0;

            rx.setPattern("<ul title=\"Cover Arts\">(.*)</ul>");
            rx.setMinimal(true);
            while ((pos = rx.indexIn(datoshtml, pos)) != -1) {
                 ++count;
                 rawmatches.append(datoshtml.mid(pos, rx.matchedLength()));
                 pos += rx.matchedLength();
            }

            rx.setPattern("<li>(.*)</li>");
            QStringList::iterator i;
            for (i = rawmatches.begin(); i != rawmatches.end(); ++i) {
                pos = 0;
                while ((pos = rx.indexIn(*i, pos)) != -1)
                {
                    ++count;
                    tmp = (*i).mid(pos, rx.matchedLength());
                    pos += rx.matchedLength();
                    int j = tmp.indexOf("<a href");
                    tmp.remove(0, j+9);
                    j = tmp.indexOf("\">");
                    tmp.remove(j, tmp.length() - j);
                    d->foundedCovers.append(tmp);
                }
            }*/

            QJsonDocument jsonResponse = QJsonDocument::fromJson(datoshtml.toUtf8());

            foreach(const QVariant &itemJson, jsonResponse.object().toVariantMap()["items"].toList())
            {
                QVariantMap itemMap = itemJson.toMap();
                d->foundedCovers.append(itemMap["link"].toString());
            }

            //for (int i=0; i<map.count(); ++i)
            //    d->foundedCovers.append(map.v);


            if ( d->foundedCovers.count() > 0 )
            {
                d->populateItems();

                d->curDown = 0;
                d->searchTry = 3;
                QString url = d->foundedCovers.at(0);
                reply = datos->get(QNetworkRequest(QUrl(url)));

            }

        }
        emit searchFinished();
    }
    else if ( d->searchTry == 3 )
    {
        QImage image = QImage::fromData(reply->readAll());
        QString path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/" + hash(d->foundedCovers.at(0)) + ".jpeg";
        image.save(path, "JPEG");
        d->items.at(d->curDown)->art = path;
        emit(d->q->dataChanged(d->q->index(d->curDown) , d->q->index(d->curDown)));

        if (d->foundedCovers.count()>0)
            d->foundedCovers.removeFirst();

        if (d->foundedCovers.count()>0) {
            d->curDown = d->curDown +1;
            d->searchTry = 3;
            QString url = d->foundedCovers.at(0);
            reply = datos->get(QNetworkRequest(QUrl(url)));
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
    QString th2 = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/media-art/album-"+ doubleHash(artist, album) + ".jpeg";

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
