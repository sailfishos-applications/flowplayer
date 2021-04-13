#include "meta.h"
#include <QFileInfo>

#include "taglib/mpeg/mpegfile.h"
#include "taglib/flac/flacfile.h"
#include "taglib/toolkit/tlist.h"
#include "taglib/ogg/vorbis/vorbisfile.h"
#include "taglib/mp4/mp4file.h"
#include "taglib/riff/wav/wavfile.h"
#include "taglib/ogg/speex/speexfile.h"
#include "taglib/ogg/flac/oggflacfile.h"
#include "taglib/riff/aiff/aifffile.h"
#include "taglib/wavpack/wavpackfile.h"
#include "taglib/trueaudio/trueaudiofile.h"
#include "taglib/asf/asffile.h"
#include "taglib/ape/apefile.h"
#include "taglib/mpc/mpcfile.h"
#include "taglib/mod/modfile.h"
#include "taglib/xm/xmfile.h"

Meta::Meta(QQuickItem *parent)
    : QQuickItem(parent)
{


}

void Meta::setFile(QString file, QString update)
{
    //QString f = file.remove("file://");
    //tagFile = new TagLib::FileRef(f.toUtf8());

    TagLib::File* tf = getFileByMimeType(file.remove("file://"));

    if(!tf) return;

    currentFile = reemplazar1(file.remove("file://"));

    tagFile = new TagLib::FileRef(tf);

    if (update=="true")
    {
        m_title = QString::fromUtf8(tagFile->tag()->title().toCString(true));
        m_album = QString::fromUtf8(tagFile->tag()->album().toCString(true));
        m_artist = QString::fromUtf8(tagFile->tag()->artist().toCString(true));
        m_year = QString::number(tagFile->tag()->year());
        m_filename = QFileInfo(file.remove("file://")).filePath().remove("/home/nemo/"); + "/" + QFileInfo(file.remove("file://")).fileName();
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
