#include "meta.h"
#include <QFileInfo>
#include <QUrl>

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

Meta::Meta(QQuickItem *parent)
    : QQuickItem(parent)
{


}

void Meta::setFile(QString file, QString update)
{
    QString path = file.startsWith(QString::fromLatin1("file://")) ? QUrl(file).toLocalFile() : file;
    //QString f = file.remove("file://");
    //tagFile = new TagLib::FileRef(f.toUtf8());

    TagLib::File* tf = getFileByMimeType(path);

    if(!tf) return;

    currentFile = reemplazar1(path);

    tagFile = new TagLib::FileRef(tf);

    if (update=="true")
    {
        m_title = QString::fromUtf8(tagFile->tag()->title().toCString(true));
        m_album = QString::fromUtf8(tagFile->tag()->album().toCString(true));
        m_artist = QString::fromUtf8(tagFile->tag()->artist().toCString(true));
        m_year = QString::number(tagFile->tag()->year());
        m_filename = path;
        emit dataChanged();
    }
}

void Meta::setData(QString title, QString artist, QString album, QString year)
{
    title = title.trimmed();
    artist = artist.trimmed();
    album = album.trimmed();
    year = year.trimmed();

    if (title=="") title = QFileInfo(currentFile).baseName();
    if (artist=="") artist = tr("Unknown artist");
    if (album=="") album = tr("Unknown album");


    QString qr = QString("UPDATE tracks SET title='%1', artist='%2', album='%3', year=%4 where url='%5'")
                .arg(reemplazar1(title)).arg(reemplazar1(artist)).arg(reemplazar1(album))
                .arg(year.toInt()).arg(currentFile);
    executeQuery(qr);

    qr = QString("UPDATE playlists SET title='%1', artist='%2', album='%3', year=%4 where url='%5'")
                .arg(reemplazar1(title)).arg(reemplazar1(artist)).arg(reemplazar1(album))
                .arg(year.toInt()).arg(currentFile);
    executeQuery(qr);

    qr = QString("UPDATE queue SET title='%1', artist='%2', album='%3', year=%4 where url='%5'")
                .arg(reemplazar1(title)).arg(reemplazar1(artist)).arg(reemplazar1(album))
                .arg(year.toInt()).arg(currentFile);
    executeQuery(qr);


    tagFile->tag()->setTitle(title.toStdWString());
    tagFile->tag()->setArtist(artist.toStdWString());
    tagFile->tag()->setAlbum(album.toStdWString());
    tagFile->tag()->setYear(year.toInt());

    tagFile->save();

    m_title = QString::fromUtf8(tagFile->tag()->title().toCString(true));
    m_album = QString::fromUtf8(tagFile->tag()->album().toCString(true));
    m_artist = QString::fromUtf8(tagFile->tag()->artist().toCString(true));
    m_year = QString::number(tagFile->tag()->year());

    emit dataChanged();
}

void Meta::setAlbumData(QString artist, QString album, QString year)
{
    artist = artist.trimmed();
    album = album.trimmed();
    year = year.trimmed();

    QString qr = QString("UPDATE tracks SET artist='%1', album='%2', year=%3 where url='%4'")
                .arg(reemplazar1(artist)).arg(reemplazar1(album)).arg(year.toInt()).arg(currentFile);
    executeQuery(qr);

    qr = QString("UPDATE playlists SET artist='%1', album='%2' where url='%3'")
                .arg(reemplazar1(artist)).arg(reemplazar1(album)).arg(currentFile);
    executeQuery(qr);

    qr = QString("UPDATE queue SET artist='%1', album='%2' where url='%3'")
                .arg(reemplazar1(artist)).arg(reemplazar1(album)).arg(currentFile);
    executeQuery(qr);

    tagFile->tag()->setArtist(artist.toStdWString());
    tagFile->tag()->setAlbum(album.toStdWString());
    tagFile->tag()->setYear(year.toInt());

    tagFile->save();

    m_album = QString::fromUtf8(tagFile->tag()->album().toCString(true));
    m_artist = QString::fromUtf8(tagFile->tag()->artist().toCString(true));
    m_year = QString::number(tagFile->tag()->year());

    emit dataAlbumChanged();

}

TagLib::File* Meta::getFileByMimeType(QString file)
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

QString Meta::reemplazar1(QString data)
{
    data.replace("&","&amp;");
    data.replace("<","&lt;");
    data.replace(">","&gt;");
    data.replace("\"","&quot;");
    data.replace("\'","&apos;");
    return data;
}

QString Meta::reemplazar2(QString data)
{
    data.replace("&amp;", "&");
    data.replace("&lt;", "<");
    data.replace("&gt;", ">");
    data.replace("&quot;", "\"");
    data.replace("&apos;", "\'");
    return data;
}
