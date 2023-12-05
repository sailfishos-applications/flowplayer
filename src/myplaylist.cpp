#include "myplaylist.h"
#include <QDesktopServices>
//#include <QDeclarativeEngine>
//#include <QDeclarativeView>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QIcon>
#include <QStringList>
#include <QCryptographicHash>
#include <QXmlStreamReader>
#include <QStandardPaths>

extern bool isDBOpened;

struct MyPlaylistItem
{
    QString artist;
    QString album;
    QString title;
    QString image;
    QString link;
    QString duration;
};

class MyPlaylist::MyPlaylistPrivate
{
public:
    MyPlaylistPrivate(MyPlaylist * parent);
    ~MyPlaylistPrivate();
    void populateItems();
    QList <MyPlaylistItem * > items;
    MyPlaylist * const q;

};

MyPlaylist::MyPlaylistPrivate::MyPlaylistPrivate(MyPlaylist * parent) :
        q(parent)
{

}

MyPlaylist::MyPlaylistPrivate::~MyPlaylistPrivate()
{
    while(!items.isEmpty())
    {
        delete items.takeFirst();
    }
}


void MyPlaylist::loadList(QString list)
{
    listado.clear();

    if (!isDBOpened) openDatabase();

    QString qr;
    if (list=="00000000000000000000")
        qr = "select artist, album, title, duration, url from queue";
    else
        qr = QString("select artist, album, title, duration, url "
                     "from playlists where playlist='%1'").arg(xmlin(list));

    QSqlQuery query = getQuery(qr);

    while( query.next() )
    {
        QString dato1, dato2, dato3, dato4, dato5;
        dato1 = query.value(0).toString();
        if (dato1=="") dato1 = tr("Unknown artist");
        dato2 = query.value(1).toString();
        if (dato2=="") dato2 = tr("Unknown album");
        dato3 = query.value(2).toString();
        if (dato3=="") dato3 = QFileInfo(dato3).baseName();
        dato4 = query.value(3).toString();
        dato5 = query.value(4).toString();

        QStringList l1;
        l1.append(xmlout(dato1));
        l1.append(xmlout(dato2));
        l1.append(xmlout(dato3));
        l1.append(dato4);
        l1.append(xmlout(dato5));
        if ( dato5!="")
            listado.append(l1);
    }

    d->populateItems();
}

QString MyPlaylist::clear(QString data)
{
    QString str = data;
    str.remove("urn:album:");
    str.remove("urn:artist:");
    str.remove("!");
    str.remove("&");
    str.remove("+");
    str.remove("\'");
    str.remove("\/");
    str.replace("<","(");
    str.replace(">",")");
    str.remove(QRegExp("\([\(].*[\)]\)"));
    str.remove("(");
    str.remove(")");
    str.replace("  "," ").replace("  "," ");
    return str.trimmed();
}

QString MyPlaylist::getThumbnail(QString artist, QString album)
{
    if ( artist.startsWith("urn:artist:") )
        artist.remove("urn:artist:");

    if ( album.startsWith("urn:album:") )
        album.remove("urn:album:");

    QString art = artist.toLower();
    QString alb = album.toLower();

    QCryptographicHash md(QCryptographicHash::Md5);
    QCryptographicHash md2(QCryptographicHash::Md5);

    QByteArray ba = clear(art).toUtf8();
    md.addData(ba);

    QByteArray ba2 = clear(alb).toUtf8();
    md2.addData(ba2);

    QString th1 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/62/album-" + QString(md.result().toHex().constData()) + "-" + QString(md2.result().toHex().constData()) + ".jpeg";

    if ( QFileInfo(th1).exists() )
        return "file://" + th1;
    else
        return "qrc:/images/nocover.jpg";

}


void MyPlaylist::MyPlaylistPrivate::populateItems()
{
    //QStringList lista1, lista2;


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
            items.append(new MyPlaylistItem);

        q->endInsertRows();
    }

    QListIterator<QStringList> entriesIterator (q->listado);
    QListIterator<MyPlaylistItem *> itemsIterator(items);

    int k= 0;
    while(entriesIterator.hasNext() && itemsIterator.hasNext())
    {
        MyPlaylistItem * item = itemsIterator.next();
        item->artist = q->listado[k][0];
        item->album = q->listado[k][1];
        item->title =  q->listado[k][2];
        item->image = q->getThumbnail(item->artist, item->album);
        item->duration = q->listado[k][3];
        item->link = q->listado[k][4];
        ++k;
        //qDebug() << item->name;
    }

    if(entriesCount != itemsCount)
    {
        emit(q->countChanged());
    }
    emit(q->dataChanged(q->index(0) , q->index(entriesCount - 1)));
    emit q->loaded();

}

MyPlaylist::MyPlaylist(QObject * parent) :
        QAbstractListModel(parent) , d (new MyPlaylistPrivate(this))
{
    // Roles
    QHash <int, QByteArray> roles;
    roles.insert(ArtistRole , "artist");
    roles.insert(AlbumRole , "album");
    roles.insert(TitleRole , "title");
    roles.insert(ImageRole , "image");
    roles.insert(LinkRole , "link");
    roles.insert(DurationRole , "duration");
    d->q->setRoleNames(roles);
}

QHash<int, QByteArray> MyPlaylist::roleNames() const {
    return m_roles;
}

void MyPlaylist::setRoleNames(const QHash<int, QByteArray>& roles) {
    m_roles = roles;
}

MyPlaylist::~MyPlaylist()
{
    delete d;
}

int MyPlaylist::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent)
    return d->items.size();
}

QVariant MyPlaylist::data(const QModelIndex & index , int role) const
{
    if(index.row() < 0 or index.row() > count())
    {
        return QVariant();
    }

    MyPlaylistItem * item = d->items.at(index.row());
    switch(role)
    {
    case ArtistRole:
        return item->artist;
        break;
    case AlbumRole:
        return item->album;
        break;
    case TitleRole:
        return item->title;
        break;
    case ImageRole:
        return item->image;
        break;
    case LinkRole:
        return item->link;
        break;
    case DurationRole:
        return item->duration;
        break;
    default:
        return QVariant();
        break;
    }
}

int MyPlaylist::count() const
{
    return rowCount();
}

QString MyPlaylist::xmlin(QString data)
{
    QString tmp = data;
    tmp.replace("&", "&amp;");
    tmp.replace("\"", "&quot;");
    tmp.replace("<", "&lt;");
    tmp.replace(">", "&gt;");
    tmp.replace("\'", "&apos;");
    return tmp;
}

QString MyPlaylist::xmlout(QString data)
{
    QString tmp = data;
    tmp.replace("&amp;", "&");
    tmp.replace("&quot;", "\"");
    tmp.replace("&lt;", "<");
    tmp.replace("&gt;", ">");
    tmp.replace("&apos;", "\'");
    return tmp;
}

