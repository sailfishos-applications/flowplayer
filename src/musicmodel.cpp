#include "musicmodel.h"

#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QIcon>
#include <QStringList>
#include <QXmlStreamReader>
#include <QSettings>
#include <QCryptographicHash>

extern bool isDBOpened;


struct MusicModelItem
{
    QString title, artist, url, album;
    int duration, track, year;

};

class MusicModel::MusicModelPrivate
{
public:
    MusicModelPrivate(MusicModel * parent);
    ~MusicModelPrivate();
    void populateItems();
    QList <MusicModelItem * > items;
    MusicModel * const q;
};

MusicModel::MusicModelPrivate::MusicModelPrivate(MusicModel * parent) :
        q(parent)
{

}

MusicModel::MusicModelPrivate::~MusicModelPrivate()
{
    while(!items.isEmpty())
    {
        delete items.takeFirst();
    }
}


void MusicModel::loadData(QString artist, QString album, QString various)
{

    album = reemplazar1(album);
    artist = reemplazar1(artist);

    if (album==tr("Unknown album")) album="";
    if (artist==tr("Unknown artist")) artist="";

    listado.clear();

    if (!isDBOpened) openDatabase();

    QSettings sets;
    QString torder = sets.value("TrackOrder", "title").toString();
    QString order;
    if (torder=="title") order="title";
    else if (torder=="number") order="tracknum";
    else if (torder=="filename") order="url";


    QString qr;

    if (artist=="ALL" && album=="ALL" && various=="ALL")
        qr = QString("select title, artist, duration, tracknum, url, album, year "
                     "from tracks order by title");

    else if (various=="1")
        qr = QString("select title, artist, duration, tracknum, url, album, year "
                     "from tracks where artist='%1' and album='%2' order by %3")
                    .arg(artist).arg(album).arg(order);

    else
        qr = QString("select title, artist, duration, tracknum, url, album, year "
                     "from tracks where album='%1' order by %3")
                     .arg(album).arg(order);


    QSqlQuery query = getQuery(qr);

    while( query.next() )
    {
        QString dato1, dato2, dato3, dato4, dato5, dato6, dato7;
        dato1 = query.value(0).toString();
        dato2 = query.value(1).toString();
        dato3 = query.value(2).toString();
        dato4 = query.value(3).toString();
        dato5 = query.value(4).toString();
        dato6 = query.value(5).toString();
        dato7 = query.value(6).toString();

        if (dato1=="") dato1 = QFileInfo(dato5).baseName();
        if (dato2=="") dato2 = tr("Unknown artist");
        //if (dato6=="") dato6 = tr("Unknown album");

        QStringList l1;
        l1.append(reemplazar2(dato1));
        l1.append(reemplazar2(dato2));
        l1.append(dato3);
        l1.append(dato4);
        l1.append(reemplazar2(dato5));
        l1.append(reemplazar2(dato6));
        l1.append(dato7);
        if ( dato4!="")
            listado.append(l1);
    }

    d->populateItems();
}

void MusicModel::clearMusic()
{
    listado.clear();
}


void MusicModel::MusicModelPrivate::populateItems()
{
    int itemsCount = items.count();
    int entriesCount = q->listado.count();

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
            items.append(new MusicModelItem);

        q->endInsertRows();
    }

    QListIterator<QStringList> entriesIterator (q->listado);
    QListIterator<MusicModelItem *> itemsIterator(items);

    int k= 0;
    q->m_time = 0;
    while(entriesIterator.hasNext() && itemsIterator.hasNext())
    {
        MusicModelItem * item = itemsIterator.next();
        item->title = q->listado[k][0];
        item->artist = q->listado[k][1];
        item->duration = q->listado[k][2].toInt();
        item->track = q->listado[k][3].toInt();
        item->url = q->listado[k][4];
        item->album = q->listado[k][5];
        item->year = q->listado[k][6].toInt();
        q->m_time = q->m_time + item->duration;
        ++k;
    }

    emit(q->timeChanged());
    emit(q->countChanged());
    emit(q->dataChanged(q->index(0) , q->index(entriesCount - 1)));

    q->m_loaded = true;
    emit (q->loadChanged());

}

void MusicModel::clearList()
{
    m_loaded = false;
    emit (d->q->loadChanged());

    listado.clear();
    emit d->q->beginResetModel();
    d->items.clear();
    emit d->q->endResetModel();
}

MusicModel::MusicModel(QObject * parent) :
        QAbstractListModel(parent) , d (new MusicModelPrivate(this))
{
    QHash <int, QByteArray> roles;
    roles.insert(TitleRole , "title");
    roles.insert(ArtistRole , "artist");
    roles.insert(DurationRole , "duration");
    roles.insert(TrackRole , "track");
    roles.insert(UrlRole , "url");
    roles.insert(AlbumRole , "album");
    roles.insert(YearRole , "year");
    d->q->setRoleNames(roles);

}

QHash<int, QByteArray> MusicModel::roleNames() const {
    return m_roles;
}

void MusicModel::setRoleNames(const QHash<int, QByteArray>& roles) {
    m_roles = roles;
}

MusicModel::~MusicModel()
{
     delete d;
}

int MusicModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent)
    return d->items.size();
}

QVariant MusicModel::data(const QModelIndex & index , int role) const
{
    if(index.row() < 0 or index.row() > count())
    {
        return QVariant();
    }

    MusicModelItem * item = d->items.at(index.row());
    switch(role)
    {
    case TitleRole:
        return item->title; break;
    case ArtistRole:
        return item->artist; break;
    case DurationRole:
        return item->duration; break;
    case TrackRole:
        return item->track; break;
    case UrlRole:
        return item->url; break;
    case AlbumRole:
        return item->album; break;
    case YearRole:
        return item->year; break;

    default:
        return QVariant();
        break;
    }
}

int MusicModel::count() const
{
    return rowCount();
}

QString MusicModel::reemplazar1(QString data)
{
    data.replace("&","&amp;");
    data.replace("<","&lt;");
    data.replace(">","&gt;");
    data.replace("\"","&quot;");
    data.replace("\'","&apos;");
    return data;
}

QString MusicModel::reemplazar2(QString data)
{
    data.replace("&amp;", "&");
    data.replace("&lt;", "<");
    data.replace("&gt;", ">");
    data.replace("&quot;", "\"");
    data.replace("&apos;", "\'");
    return data;
}
