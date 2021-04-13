#include "myplaylists.h"

#include <QDesktopServices>
//#include <QDeclarativeEngine>
//#include <QDeclarativeView>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QIcon>
#include <QStringList>
#include <QCryptographicHash>

extern bool isDBOpened;

struct MyPlaylistsItem
{
    QString name;
    QString count;

};

class MyPlaylists::MyPlaylistsPrivate
{
public:
    MyPlaylistsPrivate(MyPlaylists * parent);
    ~MyPlaylistsPrivate();
    void populateItems();
    QList <MyPlaylistsItem * > items;
    MyPlaylists * const q;

};

MyPlaylists::MyPlaylistsPrivate::MyPlaylistsPrivate(MyPlaylists * parent) :
        q(parent)
{

}

MyPlaylists::MyPlaylistsPrivate::~MyPlaylistsPrivate()
{
    while(!items.isEmpty())
    {
        delete items.takeFirst();
    }
}


void MyPlaylists::loadMyPlaylists()
{
    emit d->q->beginResetModel();
    d->items.clear();
    emit d->q->endResetModel();
    d->populateItems();
}

void MyPlaylists::MyPlaylistsPrivate::populateItems()
{
    if (!isDBOpened) openDatabase();

    QList<QStringList> lista;

    QSqlQuery query = getQuery(QString("select title, count(distinct title) as songs from queue"));

    while( query.next() )
    {
        QString dato1 = "00000000000000000000";
        QString dato2 = query.value(1).toString();
        QStringList data;
        data << q->xmlout(dato1) << dato2;
        lista.append(data);
    }

    query = getQuery(QString("select playlist, (case when (title='') then 0"
                             " else (count(distinct title)) end) as status from playlists group by playlist"));

    while( query.next() )
    {
        QString dato1 = query.value(0).toString();
        QString dato2 = query.value(1).toString();
        QStringList data;
        data << q->xmlout(dato1) << dato2;
        lista.append(data);
    }

    emit q->beginResetModel();
    q->beginInsertRows(QModelIndex(), 0, lista.count()-1);

    //QListIterator<QFileInfo> entriesIterator (entries);

    for (int i=0; i<lista.count(); ++i)
    {
        //QFileInfo fileInfo = entriesIterator.next();
        MyPlaylistsItem * item = new MyPlaylistsItem();
        //item->name = fileInfo.fileName().remove(".xml");
        item->name = lista.at(i).at(0);
        item->count = lista.at(i).at(1);
        qDebug() << "PLAYLIST: " << item->name << item->count;
        items.append(item);
    }

    q->endInsertRows();
    emit q->endResetModel();

    emit(q->countChanged());

}

MyPlaylists::MyPlaylists(QObject * parent) :
        QAbstractListModel(parent) , d (new MyPlaylistsPrivate(this))
{
    // Roles
    QHash <int, QByteArray> roles;
    roles.insert(NameRole , "name");
    roles.insert(CountRole , "count");
    d->q->setRoleNames(roles);
}

QHash<int, QByteArray> MyPlaylists::roleNames() const {
    return m_roles;
}

void MyPlaylists::setRoleNames(const QHash<int, QByteArray>& roles) {
    m_roles = roles;
}

MyPlaylists::~MyPlaylists()
{
    delete d;
}

int MyPlaylists::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent)
    return d->items.size();
}

QVariant MyPlaylists::data(const QModelIndex & index , int role) const
{
    if(index.row() < 0 or index.row() > count())
    {
        return QVariant();
    }

    MyPlaylistsItem * item = d->items.at(index.row());
    switch(role)
    {
    case NameRole:
        return item->name;
        break;
    case CountRole:
        return item->count;
        break;
    default:
        return QVariant();
        break;
    }
}

int MyPlaylists::count() const
{
    return rowCount();
}

QString MyPlaylists::xmlin(QString data)
{
    QString tmp = data;
    tmp.replace("\"", "&quot;");
    tmp.replace("<", "&lt;");
    tmp.replace(">", "&gt;");
    tmp.replace("\'", "&apos;");
    tmp.replace("&", "&amp;");
    return tmp;
}

QString MyPlaylists::xmlout(QString data)
{
    QString tmp = data;
    tmp.replace("&quot;", "\"");
    tmp.replace("&lt;", "<");
    tmp.replace("&gt;", ">");
    tmp.replace("&apos;", "\'");
    tmp.replace("&amp;", "&");
    return tmp;
}
