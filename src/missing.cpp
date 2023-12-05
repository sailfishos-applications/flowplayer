#include "missing.h"

//#include <QDeclarativeEngine>
//#include <QDeclarativeView>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QImage>
#include <QStringList>
#include <QXmlStreamReader>
#include <QSettings>
#include <QStandardPaths>

bool namefileLess(const QStringList &d1, const QStringList &d2)
{
    return d1.at(0) < d2.at(0); // sort by album
}

QString Missing::getThumbnail(QString data)
{
    QString th2 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/62/album-"+ data + ".jpeg";
    if ( QFileInfo(th2).exists() )
    {
        return "file://" + th2;
    }
    return "qrc:/images/nocover.jpg";

}

struct MissingItem
{
    QString artist, title, acount, songs, band, coverart, hash;
    bool isSelected;

};

class Missing::MissingPrivate
{
public:
    MissingPrivate(Missing * parent);
    ~MissingPrivate();
    void populateItems();
    QList <MissingItem * > items;
    Missing * const q;
};

Missing::MissingPrivate::MissingPrivate(Missing * parent) :
        q(parent)
{

}

Missing::MissingPrivate::~MissingPrivate()
{
    while(!items.isEmpty())
    {
        delete items.takeFirst();
    }
}


void Missing::loadData()
{
    listado.clear();

    QSqlQuery query = getQuery(QString("select album, artist, count(distinct artist) as acount, "
                            "count(distinct title) as songs from tracks group by album order by album, year"));

    while( query.next() )
    {
        QString dato1, dato2, dato3, dato4, dato5;
        dato1 = reemplazar2(query.value(0).toString());
        dato3 = query.value(2).toString();
        dato2 = dato3=="1" ? reemplazar2(query.value(1).toString()) : dato1;
        dato4 = dato3=="1" ? dato2 : tr("Various artists");
        dato5 = query.value(3).toString();


        if (dato1!=tr("Unknown album") && dato2!=tr("Unknown artist"))
        {
            QString th2 = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/media-art/album-"+ doubleHash(dato2, dato1) + ".jpeg";
            if ( ! QFileInfo(th2).exists() )
            {
                QString th3 = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/media-art/album-"+ doubleHash(dato1, dato1); + ".jpeg";
                if ( ! QFileInfo(th3).exists() )
                {
                    //qDebug() << dato1 << " doesn't exist. Adding to list";
                    QStringList l1;
                    l1.append(dato1);
                    l1.append(dato2);
                    l1.append(dato3);
                    l1.append(dato4);
                    l1.append(dato5);
                    if ( dato5!="")
                        listado.append(l1);

                }
            }
        }
    }

    qDebug() << "Missing covers: " << listado.count();

    lista.clear();
    lista = listado;
    d->q->sortData();



}

void Missing::sortData()
{
    //qSort(lista.begin(), lista.end(), namefileLess);
    d->populateItems();
}

void Missing::addData(QString artist, QString title, QString acount, QString band, QString songs)
{
    QStringList l1;
    l1.clear();
    l1.append(title);
    l1.append(artist);
    l1.append(acount);
    l1.append(band);
    l1.append(songs);
    listado.append(l1);

}

void Missing::clearMusic()
{
    listado.clear();
}

void Missing::removeData(QString res)
{



}

void Missing::startPepe()
{
    //pepe->run(QThread::LowPriority);
}

void Missing::MissingPrivate::populateItems()
{
    int entries = q->lista.count();

    emit q->beginResetModel();
    q->beginInsertRows(QModelIndex(), 0, entries-1);

    for (int i=0; i<entries; ++i)

    {
        MissingItem * item = new MissingItem();
        item->title = q->lista[i][0];
        item->artist = q->lista[i][1];
        item->acount = q->lista[i][2];
        item->band = q->lista[i][3];
        item->songs = q->lista[i][4];
        item->hash = "";
        item->coverart =  "";
        item->isSelected = true;
        //qDebug() << q->lista[k];
        items.append(item);
    }

    q->endInsertRows();
    emit q->countChanged();
    emit q->endResetModel();

    q->m_loaded = true;
    emit (q->loadChanged());

}

void Missing::clearList()
{
    m_loaded = false;
    emit (d->q->loadChanged());

    lista1.clear();
    emit d->q->beginResetModel();
    d->items.clear();
    emit d->q->endResetModel();

}

Missing::Missing(QObject * parent) :
        QAbstractListModel(parent) , d (new MissingPrivate(this))
{
    // Roles
    QHash <int, QByteArray> roles;
    roles.insert(ArtistRole , "artist");
    roles.insert(TitleRole , "title");
    roles.insert(AcountRole , "acount");
    roles.insert(SongsRole , "songs");
    roles.insert(BandRole , "band");
    roles.insert(CoverRole , "coverart");
    roles.insert(HashRole , "hash");
    roles.insert(SelRole , "isSelected");
    d->q->setRoleNames(roles);

    pepe = new WebThread();
    connect (pepe, SIGNAL(imgLoaded(QString,int)), this, SLOT(paintImg(QString,int)) );
    connect (pepe, SIGNAL(downloadDone()), this, SLOT(paintFinished()) );


}

QHash<int, QByteArray> Missing::roleNames() const {
    return m_roles;
}

void Missing::setRoleNames(const QHash<int, QByteArray>& roles) {
    m_roles = roles;
}

void Missing::paintImg(QString image, int index)
{
    //qDebug() << "LOADED " << index << " - " << image;

    if (d->items.count()>0) {
        d->items.at(index)->coverart = image;
        emit(d->q->dataChanged(d->q->index(index) , d->q->index(index)));

    }
    emit d->q->albumCoverChanged(image);

}

void Missing::paintFinished()
{
    runPepe = false;
    emit pepeChanged();
}

void Missing::reloadImage(int index)
{
    QString tmp = doubleHash(d->items.at(index)->artist, d->items.at(index)->title);
    d->items.at(index)->coverart = "";
    emit(d->q->dataChanged(d->q->index(index) , d->q->index(index)));
    d->items.at(index)->coverart = "file://" + QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/62/album-"+tmp+".jpeg";
    emit(d->q->dataChanged(d->q->index(index) , d->q->index(index)));
}

Missing::~Missing()
{
     delete d;
}

int Missing::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent)
    return d->items.size();
}

QVariant Missing::data(const QModelIndex & index , int role) const
{
    if(index.row() < 0 or index.row() > count())
    {
        return QVariant();
    }

    MissingItem * item = d->items.at(index.row());
    switch(role)
    {
    case ArtistRole:
        return item->artist; break;
    case TitleRole:
        return item->title; break;
    case AcountRole:
        return item->acount; break;
    case SongsRole:
        return item->songs; break;
    case BandRole:
        return item->band; break;
    case CoverRole:
        return item->coverart; break;
    case HashRole:
        return item->hash; break;
    case SelRole:
        return item->isSelected; break;

    default:
        return QVariant();
        break;
    }
}

int Missing::count() const
{
    return rowCount();
}

QString Missing::reemplazar1(QString data)
{
    data.replace("&","&amp;");
    data.replace("<","&lt;");
    data.replace(">","&gt;");
    data.replace("\"","&quot;");
    data.replace("\'","&apos;");
    return data;
}

QString Missing::reemplazar2(QString data)
{
    data.replace("&amp;", "&");
    data.replace("&lt;", "<");
    data.replace("&gt;", ">");
    data.replace("&quot;", "\"");
    data.replace("&apos;", "\'");
    return data;
}

void Missing::selectItem(int index)
{
    d->items.at(index)->isSelected = !d->items.at(index)->isSelected;
    emit(d->q->dataChanged(d->q->index(index) , d->q->index(index)));
}

void Missing::selectAllItems()
{
    for (int i=0; i< d->items.count(); ++i)
    {
        d->items.at(i)->isSelected = true;
        emit(d->q->dataChanged(d->q->index(i) , d->q->index(i)));
    }
}

void Missing::deselectAllItems()
{
    for (int i=0; i< d->items.count(); ++i)
    {
        d->items.at(i)->isSelected = false;
        emit(d->q->dataChanged(d->q->index(i) , d->q->index(i)));
    }
}

void Missing::addMissing(QString artist, QString album, int index)
{
    QStringList m;
    m.append(artist);
    m.append(album);
    m.append(QString::number(index));
    d->q->missingAlbums.append(m);
}

void Missing::clearMissing()
{
    d->q->missingAlbums.clear();
}

void Missing::startDownload()
{
    qDebug() << "Starting download...";
    runPepe = true;
    emit pepeChanged();

    for (int i=0; i<d->q->missingAlbums.count(); ++i)
    {
        pepe->addFile(missingAlbums[i][0], missingAlbums[i][1], missingAlbums[i][2].toInt());
    }
    d->q->pepe->canceled = false;
    pepe->checkAll();
}

void Missing::cancelAll()
{
    d->q->missingAlbums.clear();
    d->q->pepe->canceled = true;
    d->q->pepe->cancel();
    runPepe = false;
    emit pepeChanged();
}
