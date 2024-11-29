#include "datos.h"
#include "loadimage.h"

//#include <QDeclarativeEngine>
//#include <QDeclarativeView>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QIcon>
#include <QStringList>
#include <QXmlStreamReader>
#include <QSettings>
#include <QStandardPaths>

extern bool databaseWorking;
extern bool isDBOpened;

QString filtrado;
QString groupFilter;

bool namefileLessThan(const QStringList &d1, const QStringList &d2)
{
    if ( filtrado=="artist")
        return d1.at(3) < d2.at(3); // sort by band
    else
        return d1.at(0) < d2.at(0); // sort by album
}


QString Datos::getThumbnail(QString data, int index)
{
    QString th1 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/media-art/album-"+ data + ".jpeg";

    /*QString th2 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/flowplayer/62/album-"+ data + ".jpeg";

    QString th3 = data;

    if ( QFileInfo(th2).exists() )
    {
        return "file://" + th2;
    }
    else if ( QFileInfo(th1).exists() )
    {
        runPepe = true;
        pepe->addFile(th1, th3, index);
        return "loading";
    }
    else
        return "";*/

}

struct DatosItem
{
    QString artist, title, acount, songs, band, coverart, hash, album, duration, url, fav;
    bool isSelected;

};

class Datos::DatosPrivate
{
public:
    DatosPrivate(Datos * parent);
    ~DatosPrivate();
    void populateItems();
    QList <DatosItem * > items;
    Datos * const q;
};

Datos::DatosPrivate::DatosPrivate(Datos * parent) :
        q(parent)
{

}

Datos::DatosPrivate::~DatosPrivate()
{
    while(!items.isEmpty())
    {
        delete items.takeFirst();
    }
}

void Datos::setFilter(QString text)
{
    filtrado = text.toLower().trimmed();
    emit d->q->beginResetModel();
    d->items.clear();
    emit d->q->endResetModel();
    d->populateItems();
}


void Datos::loadArtists()
{
    groupFilter = "artist";
    filtrado = "";

    m_loaded = false;
    emit loadChanged();

    listado.clear();
    qDebug() << "LOADING MUSIC DATABASE FOR ARTISTS";
    if (!isDBOpened) openDatabase();

    QSqlQuery query = getQuery(QString("select artist, count(distinct album) as acount from tracks group by artist order by artist collate nocase"));

    while( query.next() )
    {
        QString dato1, dato2;
        dato1 = query.value(0).toString();
        dato2 = query.value(1).toString();

        QStringList l1;
        l1.append(reemplazar2(dato1));
        l1.append(dato2);
        listado.append(l1);

    }
    qDebug() << "DONE! " << listado.count();

    d->populateItems();

    //if (runPepe)
    //    pepe->start(QThread::LowPriority);
}

void Datos::loadAlbums(QString order)
{
    groupFilter = "album";
    filtrado = "";

    m_loaded = false;
    emit loadChanged();

    listado.clear();
    qDebug() << "LOADING MUSIC DATABASE FOR ALBUMS";
    if (!isDBOpened) openDatabase();

    QString fil;
    if (order=="album") fil = "album";
    else fil = "band";

    QSqlQuery query = getQuery(QString("select album, artist, count(distinct artist) as acount, count(distinct title) as songs, "
                            "(case when count(distinct artist)=1 then artist else '%1' end) as band "
                            "from tracks group by album order by %2, year collate nocase").arg(tr("Various artists")).arg(fil));

    while( query.next() )
    {
        QString dato1, dato2, dato3, dato4, dato5;
        dato1 = query.value(0).toString();
        dato2 = query.value(1).toString();
        dato3 = query.value(2).toString();
        dato4 = query.value(4).toString();
        dato5 = query.value(3).toString();


        QStringList l1;
        l1.append(reemplazar2(dato1));
        l1.append(reemplazar2(dato2));
        l1.append(reemplazar2(dato3));
        l1.append(reemplazar2(dato4));
        l1.append(dato5);
        if ( dato5!="")
            listado.append(l1);
    }
    qDebug() << "DONE! " << listado.count();

    d->populateItems();

    //if (runPepe)
    //    pepe->start(QThread::LowPriority);

}

void Datos::loadSongs(QString order)
{
    groupFilter = "songs";
    filtrado = "";

    m_loaded = false;
    emit loadChanged();

    listado.clear();
    qDebug() << "LOADING MUSIC DATABASE FOR SONGS";
    if (!isDBOpened) openDatabase();

    QString norder;
    if (order=="title") norder="title";
    else if (order=="number") norder="tracknum";
    else if (order=="filename") norder="url";

    QSqlQuery query = getQuery(QString("select artist, album, title, duration, url, tracknum, fav "
                                               "from tracks order by %1 collate nocase").arg(norder));

    while( query.next() )
    {
        QString dato1, dato2, dato3, dato4, dato5, dato6;
        dato5 = query.value(4).toString();
        dato1 = query.value(0).toString();
        dato2 = query.value(1).toString();
        dato3 = query.value(2).toString();
        if (dato3=="") dato3 = QFileInfo(dato5).baseName();
        dato4 = query.value(3).toString();
        dato6 = query.value(5).toString();

        QStringList l1;
        l1.append(reemplazar2(dato3));
        l1.append(reemplazar2(dato1));
        l1.append(reemplazar2(dato2));
        l1.append(dato4);
        l1.append(reemplazar2(dato5));
        l1.append(dato6);
        if ( dato5!="")
            listado.append(l1);
    }
    qDebug() << "DONE! " << listado.count();

    d->populateItems();

}

void Datos::addFilterToQueue()
{
    qDebug() << "ADDING SONGS TO QUEUE: " << d->items.count();

    if (!isDBOpened) openDatabase();

    /*for (int i=0; i<d->items.count(); ++i)
    {
        executeQuery(QString("insert into queue (url, artist, album, title, duration) select url, artist, album, title, duration "
                     "from tracks where url='%1'").arg(reemplazar1(d->items.at(i)->url)));
    }*/

    QString norder;
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/flowplayer.conf", QSettings::NativeFormat);
    QString order = settings.value("TrackOrder", "title").toString();

    if (order=="title") norder="title";
    else if (order=="number") norder="tracknum";
    else if (order=="filename") norder="url";


    QString qr = "insert into queue (url, artist, album, title, duration)";
    qr += "select url, artist, album, title, duration from tracks where (";

    for (int i=0; i<d->items.count(); ++i) {
        qDebug() << "Filtered: " << d->items.at(i)->url;
        qr += QString("url='%1'").arg(reemplazar1(d->items.at(i)->url));
        if (i<d->items.count()-1) qr += " or ";
        else if (i==d->items.count()-1) qr += ")";

    }
    qr += QString("order by %1 collate nocase").arg(norder);

    executeQuery(qr);
}

void Datos::addData(QString artist, QString title, QString acount, QString band, QString songs)
{

        //if ( url.contains(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/MyDocs/Music/") )
        //{
            QStringList l1;
            l1.clear();
            l1.append(title);
            l1.append(artist);
            l1.append(acount);
            l1.append(band);
            l1.append(songs);
            listado.append(l1);
        //}


}

void Datos::clearMusic()
{
    listado.clear();
}

void Datos::removeData(QString res)
{



}

/*void Datos::startPepe()
{
    //pepe->run(QThread::LowPriority);
}*/

void Datos::DatosPrivate::populateItems()
{
    int entries = q->listado.count();

    emit q->beginResetModel();
    //q->beginInsertRows(QModelIndex(), 0, entries-1);


    for (int i=0; i<entries; ++i)
    {
        if (filtrado=="" ||

            (groupFilter=="album" && (q->listado[i][1].toLower().contains(filtrado) ||
            q->listado[i][0].toLower().contains(filtrado))) ||

            (groupFilter=="artist" && q->listado[i][0].toLower().contains(filtrado)) ||

            (groupFilter=="songs" && (q->listado[i][0].toLower().contains(filtrado) ||
            q->listado[i][1].toLower().contains(filtrado) || q->listado[i][2].toLower().contains(filtrado))) ){

            DatosItem * item = new DatosItem();
            item->title = q->listado[i][0];

            if (groupFilter=="album") {
                item->artist = q->listado[i][1];
                item->acount = q->listado[i][2];
                item->band = q->listado[i][3];
                item->songs = q->listado[i][4];
                item->hash = doubleHash(item->acount=="1"? item->artist : item->title, item->title);
                item->coverart = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/media-art/album-"+ item->hash + ".jpeg";
                item->isSelected = false;
            } else if (groupFilter=="artist") {
                item->artist = q->listado[i][1]=="1"? tr("1 album") : tr("%1 albums").arg( q->listado[i][1].toInt());
                item->hash = hash(item->title.toLower());
                item->coverart = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/artist-"+ item->hash + ".jpeg";
            } else {
                item->artist = q->listado[i][1];
                item->album = q->listado[i][2];
                item->duration = q->listado[i][3];
                item->url = q->listado[i][4];
                item->fav = q->listado[i][5];
            }
            //qDebug() << "Adding album" << item->title;
            items.append(item);
        }
    }

    QDir d;
    d.mkdir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    //q->endInsertRows();
    emit q->countChanged();
    emit q->endResetModel();

    q->m_loaded = true;
    emit (q->loadChanged());
    qDebug() << "LISTO!";

}

void Datos::clearList()
{
    m_loaded = false;
    emit (d->q->loadChanged());

    listado.clear();
    emit d->q->beginResetModel();
    d->items.clear();
    emit d->q->endResetModel();

}

void Datos::startup()
{
    if (!isDBOpened) openDatabase();

    executeQuery("create table radio (name text, url text, id text, image text)");

    bool res = executeQuery("create table presets (name text, band1 text,  band2 text, "
                            "band3 text, band4 text, band5 text, band6 text, "
                            "band7 text, band8 text, band9 text, band10 text)");

    //qDebug() << "CREATE PRESETS TABLE: " << res;

    if (res) {
        // Presets table doesn't exists, generating presets

        QString qr = "insert into presets values('Classical','0','0','0','0','0','0','-7','-7','-7','-10')";
        qr += " union values('Club','0','0','8','6','6','6','3','0','0','0')";
        qr += " union values('Concert','-6','-1','2','3','3','3','3','1','1','1')";
        qr += " union values('Dance','10','7','2','0','0','-6','-7','-7','0','0')";
        qr += " union values('Full Bass','-8','10','10','6','2','-4','-8','-10','-11','-11')";
        qr += " union values('Full Bass and Treble','7','6','0','-7','-5','2','8','11','12','12')";
        qr += " union values('Full Treble','-10','-10','-10','-4','2','11','12','12','12','12')";
        qr += " union values('Headphones','5','11','6','-3','-2','2','5','10','12','12')";
        qr += " union values('Large Hall','10','10','6','6','0','-5','-5','-5','0','0')";
        qr += " union values('Live','-5','0','4','6','6','6','4','2','2','2')";
        qr += " union values('Party','7','7','0','0','0','0','0','0','7','7')";
        qr += " union values('Pop','-2','5','7','8','6','0','-2','-2','-1','-1')";
        qr += " union values('Reggae','0','0','0','-6','0','6','6','0','0','0')";
        qr += " union values('Rock','8','5','-6','-8','-3','4','8','11','11','11')";
        qr += " union values('Ska','-2','-5','-4','0','4','6','9','10','11','10')";
        qr += " union values('Soft','5','2','0','-2','0','4','8','10','11','12')";
        qr += " union values('Soft Rock','4','4','2','0','-4','-6','-3','0','2','9')";
        qr += " union values('Techno','8','6','0','-6','-5','0','8','10','10','9')";

        executeQuery(qr);
    }

}

QString Datos::dataInfo()
{
    QSqlQuery query = getQuery("select count(distinct album) as albums, count(distinct artist) as artists, count(distinct url) as songs from tracks");

    QString dato1 = "0", dato2 = "0", dato3 = "0";

    while( query.next() )
    {
        dato1 = query.value(0).toString();
        dato2 = query.value(1).toString();
        dato3 = query.value(2).toString();
    }
    return dato1 + "," + dato2 + "," + dato3;
}

QString Datos::getArtistsCovers()
{
    QSqlQuery query = getQuery("select artist from tracks group by artist order by artist collate nocase");

    QStringList dato1;

    while( query.next() ) {
        QString coverart = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/artist-" + hash(query.value(0).toString()) + ".jpeg";
        if (QFileInfo(coverart).exists())
            dato1.append(coverart);
    }

    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/flowplayer.conf", QSettings::NativeFormat);
    int first = settings.value("LastArtistItem", 0).toInt();

    if (first >= dato1.count()) {
        settings.setValue("LastArtistItem", 0);
        settings.sync();
    }
    else if (first < dato1.count() && first!=dato1.count())
    {
        QStringList dato2;
        for (int i=0; i<first; ++i)
            dato2 << dato1.takeFirst();
        dato1 << dato2;
    }

    return dato1.join("<||>");
}

QString Datos::getAlbumsCovers()
{
    QSqlQuery query = getQuery("select artist, album, count(distinct artist) as acount "
                               "from tracks group by album order by album collate nocase");

    QStringList dato1;

    while( query.next() ) {
        QString hash = doubleHash(query.value(2).toString()=="1"? query.value(0).toString() :
                                                                  query.value(1).toString(), query.value(1).toString());
        QString coverart = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/media-art/album-"+ hash + ".jpeg";

        if (QFileInfo(coverart).exists())
            dato1.append(coverart);
    }

    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/flowplayer.conf", QSettings::NativeFormat);
    int first = settings.value("LastAlbumItem", 0).toInt();

    if (first >= dato1.count()) {
        settings.setValue("LastAlbumItem", 0);
        settings.sync();
    }
    else if (first<dato1.count() && first!=dato1.count())
    {
        QStringList dato2;
        for (int i=0; i<first; ++i)
            dato2 << dato1.takeFirst();
        dato1 << dato2;
    }

    return dato1.join("<||>");
}

Datos::Datos(QObject * parent) :
        QAbstractListModel(parent) , d (new DatosPrivate(this))
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
    roles.insert(AlbumRole , "album");
    roles.insert(DurationRole , "duration");
    roles.insert(UrlRole , "url");
    roles.insert(FavRole , "fav");
    d->q->setRoleNames(roles);


    //pepe = new Thread();
    //connect (pepe, SIGNAL(imgLoaded(QString,int)), this, SLOT(paintImg(QString,int)) );
    //connect (pepe, SIGNAL(finished(QStringList)), parent, SLOT(paintFinished(QStringList)) );


}

QHash<int, QByteArray> Datos::roleNames() const {
    return m_roles;
}

void Datos::setRoleNames(const QHash<int, QByteArray>& roles) {
    m_roles = roles;
}

void Datos::paintImg(QString image, int index)
{
    d->items.at(index)->coverart = "file://" + QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/62/album-"+image+".jpeg";
    emit(d->q->dataChanged(d->q->index(index) , d->q->index(index)));
}

void Datos::reloadAll()
{
    for (int i=0; i<d->items.count(); ++i)
    {
        if (d->items.at(i)->coverart == "qrc:/images/nocover.jpg")
        {
            QString tmp = doubleHash(d->items.at(i)->artist, d->items.at(i)->title);
            if ( QFileInfo(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/62/album-"+tmp+".jpeg").exists() )
            {
                d->items.at(i)->coverart = "file://" + QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/62/album-"+tmp+".jpeg";
                emit(d->q->dataChanged(d->q->index(i) , d->q->index(i)));
            }
        }
    }
}

void Datos::reloadImage(int index)
{
    QString tmp = doubleHash(d->items.at(index)->artist, d->items.at(index)->title);
    d->items.at(index)->coverart = "qrc:/images/nocover.jpg";
    emit(d->q->dataChanged(d->q->index(index) , d->q->index(index)));
    d->items.at(index)->coverart = "file://" + QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/62/album-"+tmp+".jpeg";
    emit(d->q->dataChanged(d->q->index(index) , d->q->index(index)));
}

Datos::~Datos()
{
     delete d;
}

int Datos::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent)
    return d->items.size();
}

QVariant Datos::data(const QModelIndex & index , int role) const
{
    if(index.row() < 0 or index.row() > count())
    {
        return QVariant();
    }

    DatosItem * item = d->items.at(index.row());
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
    case AlbumRole:
        return item->album; break;
    case DurationRole:
        return item->duration; break;
    case UrlRole:
        return item->url; break;
    case FavRole:
        return item->fav; break;

    default:
        return QVariant();
        break;
    }
}

int Datos::count() const
{
    return rowCount();
}

QString Datos::reemplazar1(QString data)
{
    data.replace("&","&amp;");
    data.replace("<","&lt;");
    data.replace(">","&gt;");
    data.replace("\"","&quot;");
    data.replace("\'","&apos;");
    return data;
}

QString Datos::reemplazar2(QString data)
{
    data.replace("&amp;", "&");
    data.replace("&lt;", "<");
    data.replace("&gt;", ">");
    data.replace("&quot;", "\"");
    data.replace("&apos;", "\'");
    return data;
}

void Datos::selectItem(int index)
{
    d->items.at(index)->isSelected = !d->items.at(index)->isSelected;
    emit(d->q->dataChanged(d->q->index(index) , d->q->index(index)));
}

void Datos::selectAllItems()
{
    for (int i=0; i< d->items.count(); ++i)
    {
        d->items.at(i)->isSelected = true;
        emit(d->q->dataChanged(d->q->index(i) , d->q->index(i)));
    }
}

void Datos::deselectAllItems()
{
    for (int i=0; i< d->items.count(); ++i)
    {
        d->items.at(i)->isSelected = false;
        emit(d->q->dataChanged(d->q->index(i) , d->q->index(i)));
    }
}

void Datos::reloadItem(QString albumpath)
{
    for (int i=0; i<d->items.count(); ++i)
    {
        if (d->items.at(i)->coverart == albumpath)
        {
            //qDebug() << "Datos::::Updated" << albumpath;
            d->items.at(i)->coverart = "";
            emit(d->q->dataChanged(d->q->index(i) , d->q->index(i)));
            d->items.at(i)->coverart = albumpath;
            emit(d->q->dataChanged(d->q->index(i) , d->q->index(i)));
            break;
        }
    }
}

void Datos::setFav(QString filename, QString fav)
{
    if (fav=="1") fav = "NULL";
    else fav = "1";

    for (int i=0; i<d->items.count(); ++i)
    {
        if (d->items.at(i)->url == filename)
        {
            qDebug() << "Datos::::Updated" << filename << fav;
            executeQuery(QString("update tracks set fav=%1 where url='%2'").arg(fav).arg(filename));
            d->items.at(i)->fav = fav;
            emit(d->q->dataChanged(d->q->index(i) , d->q->index(i)));
            break;
        }
    }
}
