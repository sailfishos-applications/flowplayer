#include "datareader.h"
#include "globalutils.h"

#include <mpegfile.h>
#include <flacfile.h>
#include <tlist.h>
#include <vorbisfile.h>
#include <opusfile.h>
#include <mp4file.h>
#include <wavfile.h>
#include <speexfile.h>
#include <oggflacfile.h>
#include <aifffile.h>
#include <wavpackfile.h>
#include <trueaudiofile.h>
#include <asffile.h>
#include <apefile.h>
#include <mpcfile.h>
#include <modfile.h>
#include <xmfile.h>


#include <QDir>
#include <QDirIterator>
#include <QString>
#include <QStringList>
#include <QSettings>
#include <QDebug>
#include <QStandardPaths>

extern bool databaseWorking;
extern bool isDBOpened;

DataReader::DataReader()
{

}

void DataReader::openDB()
{
    if (!isDBOpened) openDatabase();

    if (isDBOpened)
    {
        executeQuery("create table tracks (url text, artist text, album text, "
                   "title text, year integer, tracknum integer, duration integer, fav integer)");

        executeQuery("create table playlists (playlist text, url text, artist text, album text, "
                   "title text, duration integer)");

        executeQuery("create table queue (url text, artist text, album text, "
                   "title text, duration integer)");

        executeQuery("create table temp (url text, artist text, album text, "
                   "title text, year integer, tracknum integer, duration integer, fav integer)");

        //executeQuery(QString("delete from tracks"));

    }

}

void DataReader::insertData(QString url, QString artist, QString album,
                            QString title, int year, int tracknum, int duration)
{

    QVariantMap m;
    m.insert("url", url);
    m.insert("artist", artist);
    m.insert("album", album);
    m.insert("title", title);
    m.insert("year", year);
    m.insert("tracknum", tracknum);
    m.insert("duration", duration);

    if (favFiles.indexOf(url)>-1)
        m.insert("fav", "1");
    else
        m.insert("fav", "");

    map.append(m);

    if (map.count()==50)
    {
        saveData();
        map.clear();
    }


    //bool res = executeQuery(QString("insert into tracks values('%1','%2','%3','%4', %5, %6, %7)")
    //                                  .arg(url).arg(artist).arg(album).arg(title).arg(year).arg(tracknum).arg(duration));

    //qDebug() << "ADDING " << url << res;

}

void DataReader::saveData()
{
    if (map.count()==0)
        return;

    if (!isDBOpened) openDatabase();

    QString qr = "insert into tracks";

    for (int i=0; i<map.count(); ++i)
    {
        if (i==0)
        {
            qr += QString(" select '%1' as url, '%2' as artist, '%3' as album, '%4' as title, "
                    "%5 as year, %6 as tracknum, %7 as duration, '%8' as fav")
                    .arg(map.at(i).value("url","").toString())
                    .arg(map.at(i).value("artist","").toString())
                    .arg(map.at(i).value("album","").toString())
                    .arg(map.at(i).value("title","").toString())
                    .arg(map.at(i).value("year",0).toInt())
                    .arg(map.at(i).value("tracknum",0).toInt())
                    .arg(map.at(i).value("duration",0).toInt())
                    .arg(map.at(i).value("fav","").toString());
        }
        else
        {
            qr += QString(" union select '%1', '%2','%3', '%4',%5, %6, %7, '%8'")
                          .arg(map.at(i).value("url","").toString())
                          .arg(map.at(i).value("artist","").toString())
                          .arg(map.at(i).value("album","").toString())
                          .arg(map.at(i).value("title","").toString())
                          .arg(map.at(i).value("year",0).toInt())
                          .arg(map.at(i).value("tracknum",0).toInt())
                          .arg(map.at(i).value("duration",0).toInt())
                          .arg(map.at(i).value("fav","").toInt());
        }
    }

    bool res = executeQuery(qr);

    qDebug() << "APPENDING ITEMS: " << res;
}

QString DataReader::reemplazar1(QString data)
{
    data.replace("&","&amp;");
    data.replace("<","&lt;");
    data.replace(">","&gt;");
    data.replace("\"","&quot;");
    data.replace("\'","&apos;");
    return data;
}

QString DataReader::reemplazar2(QString data)
{
    data.replace("&amp;", "&");
    data.replace("&lt;", "<");
    data.replace("&gt;", ">");
    data.replace("&quot;", "\"");
    data.replace("&apos;", "\'");
    return data;
}

TagLib::File* DataReader::getFileByMimeType(QString file)
{
    file = file.remove("file://");
    QString str = file.toLower();

    if(str.endsWith(".mp3")) {
        return new TagLib::MPEG::File(file.toUtf8());
    } else if(str.endsWith(".flac")) {
        return new TagLib::FLAC::File(file.toUtf8());
    } else if(str.endsWith(".ogg")) {
        return new TagLib::Ogg::Vorbis::File(file.toUtf8());
    } else if(str.endsWith(".opus")) {
        return new TagLib::Ogg::Opus::File(file.toUtf8());
    } else if(str.endsWith(".wav")) {
        return new TagLib::RIFF::WAV::File(file.toUtf8());
    } else if(str.endsWith(".m4a")) {
        return new TagLib::MP4::File(file.toUtf8());
    } else if(str.endsWith(".wma") ||
              str.endsWith(".asf")) {
        return new TagLib::ASF::File(file.toUtf8());
    } else if(str.endsWith(".mod")) {
        return new TagLib::Mod::File(file.toUtf8());
    } else if(str.endsWith(".xm")) {
        return new TagLib::XM::File(file.toUtf8());
    }
    return 0;
}

void DataReader::readFile(QString file)
{
    file.remove("file://");
    TagLib::File* tf = getFileByMimeType(file);

    QString error = "";
    if (tf)
    {
        tagFile = new TagLib::FileRef(tf);

        if (!tagFile->isNull())
        {
            m_title = QString::fromUtf8(tagFile->tag()->title().toCString(true));
            m_album = QString::fromUtf8(tagFile->tag()->album().toCString(true));
            m_artist = QString::fromUtf8(tagFile->tag()->artist().toCString(true));
            m_year = QString::number(tagFile->tag()->year());
            m_tracknum = QString::number(tagFile->tag()->track());

            if (m_title=="") m_title = QFileInfo(file).baseName();

            // if we have artist and album, we check for a cover image.
            if (m_artist != "" && m_album != "") {
                QFileInfo info(file);
                QDirIterator iterator(info.dir());
                while (iterator.hasNext()) {
                    iterator.next();
                    // we are explicit about two common factors, the type JPEG (ToDo: add PNG
                    // throughout all C++ source files, see issue #78), and basename cover or folder
                    if (iterator.fileInfo().isFile()) {
                        if (  (iterator.fileInfo().suffix() == "jpeg" ||
                               iterator.fileInfo().suffix() == "jpg") &&
                              // See ToDo above: (… ||
                              //                  iterator.fileInfo().suffix() == "png") &&
                              (iterator.fileInfo().baseName() == "cover" ||
                               iterator.fileInfo().baseName() == "folder")  ) {
                            QString th2 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
                                          "/media-art/album-" + doubleHash(m_artist, m_album) + ".jpeg";
                            qDebug() << "COPYING FILE ART: " << iterator.filePath() << m_artist << m_album;
                            QFile::copy(iterator.filePath(), th2);
                        }
                    }
                }
            }

            if (m_artist=="") m_artist = tr("Unknown artist");
            if (m_album=="") m_album = tr("Unknown album");

            TagLib::AudioProperties *properties = tf->audioProperties();

            m_duration = QString::number(properties->length());

            insertData(reemplazar1(file), reemplazar1(m_artist), reemplazar1(m_album),
                       reemplazar1(m_title), m_year.toInt(), m_tracknum.toInt(), m_duration.toInt());

        }
        else
        {
            error = "INVALID TAG";
        }
        delete tagFile;
    }
    else
    {
        error = "INVALID FILE";
    }
    qDebug() << "PROCESSING FILE: " << file << (error==""? "OK" : error);

}

void DataReader::addFile(QString file)
{
    files.append(file);
}

void DataReader::run()
{
    forceFinish = false;

    files.clear();
    favFiles.clear();
    map.clear();

    QSettings settings;
    QStringList folders = settings.value("Folders","").toString().split("<separator>");
    folders.removeAll("");

    for (int i=0; i<folders.count(); ++i)
    {
        QDir dir(folders.at(i));
        QDirIterator iterator(dir.absolutePath(), QDirIterator::Subdirectories);
        while (iterator.hasNext()) {
            iterator.next();
            if (!iterator.fileInfo().isDir()) {
                if ( iterator.filePath().toLower().endsWith(".mp3") ||
                     iterator.filePath().toLower().endsWith(".m4a") ||
                     iterator.filePath().toLower().endsWith(".flac") ||
                     iterator.filePath().toLower().endsWith(".ogg") ||
                     iterator.filePath().toLower().endsWith(".opus") ||
                     iterator.filePath().toLower().endsWith(".wma") ||
                     iterator.filePath().toLower().endsWith(".asg") ||
                     iterator.filePath().toLower().endsWith(".wav") )
                {
                    files.append(iterator.filePath());
                    if (forceFinish) {
                        emit finished();
                        break;
                    }
                }
                if ( (iterator.filePath().toLower().endsWith(".mod") ||
                     iterator.filePath().toLower().endsWith(".xm")) &&
                     QFileInfo("/usr/lib/libmodplug.so.1").exists() )
                {
                    files.append(iterator.filePath());
                    if (forceFinish) {
                        emit finished();
                        break;
                    }
                }
            }
        }

        if (forceFinish) {
            emit finished();
            break;
        }

    }

    files.removeDuplicates();
    emit total(files.count());

    QSqlQuery query = getQuery("select * from tracks where fav=1");
    while( query.next() )
    {
        favFiles.append(query.value(0).toString());
    }

    executeQuery(QString("delete from tracks"));

    if (forceFinish) {
        emit finished();
        return;
    }

    for (int i=0; i<files.count(); ++i)
    {
        emit percent(i);
        //qDebug() << "PROCESSING ITEM " << files.at(i);
        readFile(files.at(i));

        if (forceFinish) {
            emit finished();
            break;
        }

    }
    saveData();

    executeQuery("create table tmp as select * from tracks group by url");
    executeQuery("drop table tracks");
    executeQuery("create table tracks as select * from tmp");
    executeQuery("drop table tmp");

    files.clear();
    favFiles.clear();
    map.clear();

    emit finished();

}

void DataReader::clearFiles()
{
    files.clear();
}
