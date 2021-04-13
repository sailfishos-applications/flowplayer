#include "playlist.h"
#include <algorithm>
#include <QSettings>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QDir>

QSettings sets("cepiperez", "flowplayer");
QList<int> randomlist;
int currentItem, currentRandom;

extern bool databaseWorking;
extern bool isDBOpened;

Playlist::Playlist(QQuickItem *parent)
    : QQuickItem(parent)
{

}

int Playlist::current()
{
    return currentItem;
}

QString Playlist::active() const
{
    QString t = sets.value("Active", "false").toString();
    return t;
}

QString Playlist::unknown() const
{
    QString t = sets.value("Unknown", "false").toString();
    return t;
}

QString Playlist::currentSongArtist()
{
    if (isRandom)
        return listado[randomlist.at(currentRandom)][0];
    else
        return listado[currentItem][0];
}

QString Playlist::currentSongAlbum()
{
    if (isRandom)
        return listado[randomlist.at(currentRandom)][1];
    else
        return listado[currentItem][1];
}

QString Playlist::currentSongTitle()
{
    if (isRandom)
        return listado[randomlist.at(currentRandom)][2];
    else
        return listado[currentItem][2];
}

int Playlist::currentSongDuration()
{
    if (isRandom)
        return listado[randomlist.at(currentRandom)][3].toInt();
    else
        return listado[currentItem][3].toInt();
}

QString Playlist::nextSongArtist()
{
    int i;
    if (isRandom)
    {
        i = currentRandom +1;
        if ( i == listado.count() )
            i = 0;
        return listado[randomlist.at(i)][0];
    }
    else
    {
        i = currentItem +1;
        if ( i == listado.count() )
            i = 0;
        return listado[i][0];
    }
}

QString Playlist::nextSongAlbum()
{
    int i;
    if (isRandom)
    {
        i = currentRandom +1;
        if ( i == listado.count() )
            i = 0;
        return listado[randomlist.at(i)][1];
    }
    else
    {
        i = currentItem +1;
        if ( i == listado.count() )
            i = 0;
        return listado[i][1];
    }
}

QString Playlist::prevSongArtist()
{
    int i;
    if (isRandom)
    {
        i = currentRandom -1;
        if ( i == -1 )
            i = listado.count()-1;
        return listado[randomlist.at(i)][0];
    }
    else
    {
        i = currentItem -1;
        if ( i == -1 )
            i = listado.count()-1;
        return listado[i][0];
    }
}

QString Playlist::prevSongAlbum()
{
    int i;
    if (isRandom)
    {
        i = currentRandom -1;
        if ( i == -1 )
            i = listado.count()-1;
        return listado[randomlist.at(i)][1];
    }
    else
    {
        i = currentItem -1;
        if ( i == -1 )
            i = listado.count()-1;
        return listado[i][1];
    }
}

void Playlist::loadList(QString list)
{
    listado.clear();

    if (!isDBOpened) openDatabase();

    QSqlQuery query = getQuery("select artist, album, title, duration, url from queue");

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

    randomlist.clear();
    for (int i=0; i<listado.count(); ++i)
        randomlist.append(i);
    std::random_shuffle(randomlist.begin(), randomlist.end());
    //qDebug() << "RANDOM LIST: " << randomlist;

}

void Playlist::addToList(QString list, QString artist, QString album, QString title, QString link, QString time)
{
    //loadList(list);

    bool add = true;
    for (int f=0; f<listado.count(); f++)
    {
        if ( listado[f][4] == link )
            add = false;
    }

    if ( add == true )
    {
        qDebug() << "Adding file:" << link << " - total files: " << listado.count();
        QStringList l1;
        l1.append(artist);
        l1.append(album);
        l1.append(title);
        l1.append(time);
        l1.append(link);
        listado.append(l1);

        randomlist.clear();
        for (int i=0; i<listado.count(); ++i)
            randomlist.append(i);
        std::random_shuffle(randomlist.begin(), randomlist.end());

    }

    //saveList(list);
}

void Playlist::addAlbumToList(QString artist, QString album, QString various)
{
    if (!isDBOpened) openDatabase();

    QString qr;
    if (various=="1")
    {
        qr = QString("select artist, album, title, duration, url from tracks where album='%1' and artist='%2'")
                .arg(album).arg(artist);
    }
    else
    {
        qr = QString("select artist, album, title, duration, url from tracks where album='%1'");
    }

    QSqlQuery query = getQuery(qr);

    while( query.next() )
    {
        QString dato1, dato2, dato3, dato4, dato5;
        dato1 = query.value(0).toString();
        dato2 = query.value(1).toString();
        dato3 = query.value(2).toString();
        dato4 = query.value(3).toString();
        dato5 = query.value(4).toString();

        QStringList l1;
        l1.append(xmlout(dato1));
        l1.append(xmlout(dato2));
        l1.append(xmlout(dato3));
        l1.append(dato4);
        l1.append(xmlout(dato5));
        if ( dato5!="")
        {
            addToList("00000000000000000000", dato1, dato2, dato3, dato5, dato4);
        }

    }

}


void Playlist::saveList(QString list)
{
    //qDebug() << "Saving playlist :" << list;

    /*QString archivo;
    archivo = "/home/nemo/.config/cepiperez/playlists/" + list + ".xml";
    QFile file( archivo );
    file.open( QIODevice::Truncate | QIODevice::Text | QIODevice::ReadWrite);
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "<Playlist>\n";
    for ( int i=0; i < listado.count(); ++i)
    {
        if ( listado[i][0] != "Más..." )
        {
            out << "  <" << "Song" << " "
                << "Artist=\"" << xmlin(listado[i][0]) << "\" "
                << "Album=\"" << xmlin(listado[i][1]) << "\" "
                << "Title=\"" << xmlin(listado[i][2]) << "\" "
                << "Duration=\"" << listado[i][3] << "\" "
                << "Url=\"" << xmlin(listado[i][4]) << "\"/>\n";
        }
    }
    out << "</Playlist>\n";
    file.close();*/

    if (!isDBOpened) openDatabase();

    executeQuery(QString("delete from queue"));

    for ( int i=0; i < listado.count(); ++i)
    {
        executeQuery(QString("insert into queue values('%1','%2','%3','%4', %5)")
                       .arg(xmlin(listado[i][4])).arg(xmlin(listado[i][0]))
                       .arg(xmlin(listado[i][1])).arg(xmlin(listado[i][2]))
                       .arg(listado[i][3]));

    }


}

void Playlist::removeFromList(QString link)
{
    //qDebug() << "START REMOVING "<< link;
    QList<QStringList> newlist;

    for (int f=0; f<listado.count(); f++)
    {
        if ( listado[f][4] != link )
        {
            QStringList l1;
            l1.append(listado[f][0]);
            l1.append(listado[f][1]);
            l1.append(listado[f][2]);
            l1.append(listado[f][3]);
            l1.append(listado[f][4]);
            newlist.append(l1);
        }
    }

    listado.clear();
    for (int f=0; f<newlist.count(); f++)
    {
        QStringList l1;
        l1.append(newlist[f][0]);
        l1.append(newlist[f][1]);
        l1.append(newlist[f][2]);
        l1.append(newlist[f][3]);
        l1.append(newlist[f][4]);
        listado.append(l1);
    }

    randomlist.clear();
    for (int i=0; i<listado.count(); ++i)
        randomlist.append(i);
    std::random_shuffle(randomlist.begin(), randomlist.end());

    //qDebug() << "REMOVE OK";

}

void Playlist::removeList(QString list)
{
    if (!isDBOpened) openDatabase();

    executeQuery(QString("delete from playlists where playlist='%1").arg(xmlin(list)));

}

void Playlist::clearList(QString list)
{
    //qDebug() << "Cleaning playlist";
    listado.clear();
    //saveList(list);
    /*QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString path("/home/nemo/.config/cepiperez/flowplayer.db");
    db.setDatabaseName(path);
    db.open();

    QSqlQuery query;
    query.exec(QString("delete from queue"));*/

}

void Playlist::setCurrent(int val)
{
    currentItem = val;
    currentRandom = val;
}

QString Playlist::random()
{
    currentRandom = currentRandom+1;
    if ( currentRandom == randomlist.count() )
        currentRandom = 0;
    currentItem = randomlist.at(currentRandom);
    return listado[currentItem][4];
}

QString Playlist::next()
{
    if (isRandom)
    {
        currentRandom = currentRandom + 1;
        if ( currentRandom == listado.count() )
            currentRandom = 0;
        return listado[randomlist.at(currentRandom)][4];
    }
    else
    {
        currentItem = currentItem + 1;
        if ( currentItem == listado.count() )
            currentItem = 0;
        return listado[currentItem][4];
    }
}

QString Playlist::prev()
{
    if (isRandom)
    {
        currentRandom = currentRandom - 1;
        if ( currentRandom == -1 )
            currentRandom = listado.count()-1;
        return listado[randomlist.at(currentRandom)][4];
    }
    else
    {
        currentItem = currentItem - 1;
        if ( currentItem == -1 )
            currentItem = listado.count()-1;
        return listado[currentItem][4];
    }
}

void Playlist::changeUnknown(bool active)
{
    //qDebug() << "CHANGING UKNOWN: " << active;

    sets.setValue("Unknown", active);
    sets.sync();
}

void Playlist::changeMode(QString mode)
{
    //qDebug() << "CHANGING MODE: " << mode;

    sets.setValue("Mode", mode);
    sets.sync();
}

int Playlist::curIndex(QString song)
{
    song.remove("file://");
    int x=0;
    if (isRandom)
    {
        for (int f=0; f<listado.count(); f++)
        {
            if ( listado[randomlist.at(f)][4] == song )
                x = f;
        }
    }
    else
    {
        for (int f=0; f<listado.count(); f++)
        {
            if ( listado[f][4] == song )
                x = f;
        }
    }
    return x;
}

void Playlist::setRandom(bool val)
{
    isRandom = val;
}

QString Playlist::xmlin(QString data)
{
    QString tmp = data;
    tmp.replace("&", "&amp;");
    tmp.replace("\"", "&quot;");
    tmp.replace("<", "&lt;");
    tmp.replace(">", "&gt;");
    tmp.replace("\'", "&apos;");
    return tmp;
}

QString Playlist::xmlout(QString data)
{
    QString tmp = data;
    tmp.replace("&amp;", "&");
    tmp.replace("&quot;", "\"");
    tmp.replace("&lt;", "<");
    tmp.replace("&gt;", ">");
    tmp.replace("&apos;", "\'");
    return tmp;
}
