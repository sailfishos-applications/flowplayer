#include "loadwebimage.h"
#include "utils.h"

#include <QCryptographicHash>
#include <QUrl>
#include <qstring.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <QImage>
#include <QDebug>
#include <QDateTime>
#include <QStandardPaths>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


QString hmacSha1(QByteArray key, QByteArray baseString)
{
    int blockSize = 64; // HMAC-SHA-1 block size, defined in SHA-1 standard
    if (key.length() > blockSize) { // if key is longer than block size (64), reduce key length with SHA-1 compression
        key = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    }

    QByteArray innerPadding(blockSize, char(0x36)); // initialize inner padding with char "6"
    QByteArray outerPadding(blockSize, char(0x5c)); // initialize outer padding with char "\"
    // ascii characters 0x36 ("6") and 0x5c ("\") are selected because they have large
    // Hamming distance (http://en.wikipedia.org/wiki/Hamming_distance)

    for (int i = 0; i < key.length(); i++) {
        innerPadding[i] = innerPadding[i] ^ key.at(i); // XOR operation between every byte in key and innerpadding, of key length
        outerPadding[i] = outerPadding[i] ^ key.at(i); // XOR operation between every byte in key and outerpadding, of key length
    }

    // result = hash ( outerPadding CONCAT hash ( innerPadding CONCAT baseString ) ).toBase64
    QByteArray total = outerPadding;
    QByteArray part = innerPadding;
    part.append(baseString);
    total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha1));
    QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Sha1);
    return hashed.toBase64();
}

QString luceneEscape(QString data)
{
    data.replace("\\", "\\\\");
    data.replace("\"", "\\\"");
    return data;
}

// MusicBrainz, the Cover Art Archive and Wikimedia reject anonymous clients
QNetworkRequest webRequest(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", QString("FlowPlayer/%1 ( https://github.com/sailfishos-applications/flowplayer )").arg(VERSION).toUtf8());
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    return request;
}

// Wikidata query for the photos of the given MusicBrainz artist ids
QUrl wikidataPhotosUrl(const QStringList &ids)
{
    QString sparql = "SELECT ?mbid ?img WHERE { VALUES ?mbid { \"" + ids.join("\" \"") + "\" } ?item wdt:P434 ?mbid ; wdt:P18 ?img . }";
    return QUrl::fromEncoded("https://query.wikidata.org/sparql?format=json&query=" + QUrl::toPercentEncoding(sparql));
}

// Artist id -> photo URL (500 px wide) from a wikidataPhotosUrl() reply
QMap<QString, QString> wikidataPhotos(const QByteArray &reply)
{
    QMap<QString, QString> photos;
    foreach (const QJsonValue &binding, QJsonDocument::fromJson(reply).object().value("results").toObject().value("bindings").toArray())
    {
        QString img = binding.toObject().value("img").toObject().value("value").toString();
        img.replace(QRegExp("^http:"), "https:");
        photos.insert(binding.toObject().value("mbid").toObject().value("value").toString(), img + "?width=500");
    }
    return photos;
}

WebThread::WebThread()
{
    wdatos = new QNetworkAccessManager(this);
    connect(wdatos, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded(QNetworkReply*)));

    musicBrainzDelay = new QTimer(this);
    musicBrainzDelay->setSingleShot(true);
    connect(musicBrainzDelay, SIGNAL(timeout()), this, SLOT(sendMusicBrainzQuery()));

}

WebThread::~WebThread()
{

}

void WebThread::addFile(QString artist, QString album, int index)
{
    QStringList m;
    m.append(artist);
    m.append(album);
    m.append(QString::number(index));
    //qDebug() << "Adding missing: " << m;
    files.append(m);
}

void WebThread::run()
{

}

bool WebThread::checkInternal()
{
    bool res = false;

    QString artist = files[0][0];
    QString album = files[0][1];

    QSqlQuery query = getQuery(QString("select url from tracks where artist='%1' and album='%2' limit 1").arg(escapeForDb(artist)).arg(escapeForDb(album)));
    while( query.next() )
    {
        QString dir = QFileInfo(query.value(0).toString()).path();

        if (QFileInfo(dir + "/folder.jpg").exists())
        {
            QString th2 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/media-art/album-"+ doubleHash(artist, album) + ".jpeg";
            QImage image(dir + "/folder.jpg");
            image.save(th2, "JPEG");
            emit imgLoaded(th2, files[0][2].toInt());
            res = true;
        }

        else if (QFileInfo(dir + "/folder.jpeg").exists())
        {
            QString th2 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/media-art/album-"+ doubleHash(artist, album) + ".jpeg";
            QImage image(dir + "/folder.jpeg");
            image.save(th2, "JPEG");
            emit imgLoaded(th2, files[0][2].toInt());
            res = true;
        }

        else if (QFileInfo(dir + "/cover.jpg").exists())
        {
            QString th2 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/media-art/album-"+ doubleHash(artist, album) + ".jpeg";
            QImage image(dir + "/cover.jpg");
            image.save(th2, "JPEG");
            emit imgLoaded(th2, files[0][2].toInt());
            res = true;
        }

        if (QFileInfo(dir + "/Folder.jpg").exists())
        {
            QString th2 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/media-art/album-"+ doubleHash(artist, album) + ".jpeg";
            QImage image(dir + "/Folder.jpg");
            image.save(th2, "JPEG");
            emit imgLoaded(th2, files[0][2].toInt());
            res = true;
        }

        else if (QFileInfo(dir + "/Folder.jpeg").exists())
        {
            QString th2 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/media-art/album-"+ doubleHash(artist, album) + ".jpeg";
            QImage image(dir + "/Folder.jpeg");
            image.save(th2, "JPEG");
            emit imgLoaded(th2, files[0][2].toInt());
            res = true;
        }

        else if (QFileInfo(dir + "/Cover.jpg").exists())
        {
            QString th2 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/media-art/album-"+ doubleHash(artist, album) + ".jpeg";
            QImage image(dir + "/Cover.jpg");
            image.save(th2, "JPEG");
            emit imgLoaded(th2, files[0][2].toInt());
            res = true;
        }

    }
    return res;
}

void WebThread::checkAll()
{
    if (canceled || files.isEmpty())
        return;

    // Queued with an empty album: artist image
    if (files[0][1].isEmpty())
    {
        queryArtist();
        return;
    }

    if (checkInternal())
    {
        files.removeAt(0);

        if (files.count()>0)
            checkAll();
        else
            emit downloadDone();

        return;
    }

    searchAlbum = files[0][1];
    artistIds = "";
    artistSearched = false;
    queryMusicBrainz();
}

void WebThread::queryMusicBrainz()
{
    QString artist = files[0][0];

    QString query = "releasegroup:\"" + luceneEscape(searchAlbum) + "\"";
    if (!artistIds.isEmpty())
        query += " AND (" + artistIds + ")";
    // Compilations are queued with the album title as artist
    else if (artist != files[0][1])
        query += " AND artist:\"" + luceneEscape(artist) + "\"";

    action = "link";
    musicBrainzUrl = "https://musicbrainz.org/ws/2/release-group/?fmt=json&limit=5&query=" + QUrl::toPercentEncoding(query);
    retries = 0;
    sendMusicBrainzQuery();
}

// Release group searches only match the artist's main name, the artist
// search also matches aliases (e.g. "Masayoshi Takanaka" for 高中正義)
void WebThread::queryArtist()
{
    artistSearched = true;
    action = "artist";
    musicBrainzUrl = "https://musicbrainz.org/ws/2/artist/?fmt=json&limit=3&query=" + QUrl::toPercentEncoding("\"" + luceneEscape(files[0][0]) + "\"");
    retries = 0;
    sendMusicBrainzQuery();
}

// Artist photos come from Wikidata, linked to MusicBrainz by artist id
void WebThread::queryPhoto(const QStringList &ids)
{
    photoIds = ids;
    action = "photo";
    wdatos->get(webRequest(wikidataPhotosUrl(ids)));
}

void WebThread::sendMusicBrainzQuery()
{
    if (canceled || files.isEmpty())
        return;

    // MusicBrainz allows one request per second
    if (lastMusicBrainzQuery.isValid() && lastMusicBrainzQuery.elapsed() < 1100)
    {
        musicBrainzDelay->start(1100 - lastMusicBrainzQuery.elapsed());
        return;
    }
    lastMusicBrainzQuery.start();

    wdatos->get(webRequest(QUrl::fromEncoded(musicBrainzUrl.toLatin1())));
}

void WebThread::nextFile()
{
    files.removeAt(0);

    if (files.count()>0)
        checkAll();
    else
        emit downloadDone();
}

void WebThread::notFound(int current)
{
    emit imgLoaded("ERROR", current);

    qDebug() << "Not found: " << files[0][0] << files[0][1];

    nextFile();
}

void WebThread::downloaded(QNetworkReply *respuesta)
{
    respuesta->deleteLater();

    if (canceled)
        return;

    int current = 0;

    if (files.count()>0)
        current = files[0][2].toInt();
    else
        current = curImageIndex;

    if (respuesta->error() != QNetworkReply::NoError)
    {
        int status = respuesta->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // MusicBrainz answers 503 when it is busy or we are too fast
        if ((action == "link" || action == "artist") && status == 503 && retries < 5)
        {
            ++retries;
            musicBrainzDelay->start(3000 * retries);
            return;
        }

        // Not every release group has cover art, try the next match
        if (action == "image" && !coverLinks.isEmpty())
        {
            downloadImage(coverLinks.takeFirst());
            return;
        }

        qDebug() << "error: " << action << respuesta->error();
        emit imgLoaded("ERROR", current);

        if (files.count()>0)
            nextFile();
        else
            emit downloadDone();
    }
    else
    {

        if (action == "link")
        {
            coverLinks.clear();
            foreach (const QJsonValue &group, QJsonDocument::fromJson(respuesta->readAll()).object().value("release-groups").toArray())
            {
                if (group.toObject().value("score").toInt() >= 90)
                    coverLinks.append("https://coverartarchive.org/release-group/" + group.toObject().value("id").toString() + "/front-500");
            }

            // Edition suffixes like "(Remastered 1996)" are not part of MusicBrainz titles
            QString baseAlbum = searchAlbum;
            baseAlbum.remove(QRegExp("\\s*[\\(\\[][^\\(\\)\\[\\]]*[\\)\\]]\\s*$"));

            if (!coverLinks.isEmpty())
            {
                qDebug() << "Link for" << files[0][0] << files[0][1] << coverLinks.first();

                downloadImage(coverLinks.takeFirst());
            }
            else if (!baseAlbum.isEmpty() && baseAlbum != searchAlbum)
            {
                searchAlbum = baseAlbum;
                queryMusicBrainz();
            }
            else if (!artistSearched && files[0][0] != files[0][1])
            {
                queryArtist();
            }
            else
            {
                notFound(current);
            }

        }
        else if (action == "artist")
        {
            // Prefer artists whose name or alias is exactly the one searched:
            // "Masayoshi Takanaka" also scores 100 for "MTSCA"
            QStringList ids, exactIds;
            foreach (const QJsonValue &value, QJsonDocument::fromJson(respuesta->readAll()).object().value("artists").toArray())
            {
                QJsonObject artist = value.toObject();
                if (artist.value("score").toInt() < 90)
                    continue;

                QString id = artist.value("id").toString();
                ids.append(id);

                bool exact = artist.value("name").toString().compare(files[0][0], Qt::CaseInsensitive) == 0;
                foreach (const QJsonValue &alias, artist.value("aliases").toArray())
                    exact = exact || alias.toObject().value("name").toString().compare(files[0][0], Qt::CaseInsensitive) == 0;
                if (exact)
                    exactIds.append(id);
            }
            if (!exactIds.isEmpty())
                ids = exactIds;

            if (!ids.isEmpty() && files[0][1].isEmpty())
            {
                queryPhoto(ids);
            }
            else if (!ids.isEmpty())
            {
                for (int i = 0; i < ids.count(); ++i)
                    ids[i] = "arid:" + ids.at(i);
                artistIds = ids.join(" OR ");
                searchAlbum = files[0][1];
                queryMusicBrainz();
            }
            else
            {
                notFound(current);
            }
        }
        else if (action == "photo")
        {
            QMap<QString, QString> photos = wikidataPhotos(respuesta->readAll());

            coverLinks.clear();
            foreach (const QString &id, photoIds)
            {
                if (photos.contains(id))
                    coverLinks.append(photos.value(id));
            }

            if (!coverLinks.isEmpty())
                downloadImage(coverLinks.takeFirst());
            else
                notFound(current);
        }
        else if (action == "image")
        {
            qDebug() << "Saving image for" << files[0][0] << files[0][1];

            QString tmp = saveToDisk(respuesta);

            if (tmp == "ERROR" && !coverLinks.isEmpty())
            {
                downloadImage(coverLinks.takeFirst());
                return;
            }

            emit imgLoaded(tmp, current);

            nextFile();
        }
        else if (action == "imageextern")
        {
            qDebug() << "Saving image for" << curImage;
            QString tmp = saveToDiskExtern(respuesta);
            emit imgLoaded(tmp, current);
        }

    }

}

void WebThread::cancel()
{
    musicBrainzDelay->stop();
    coverLinks.clear();
    files.clear();
}

void WebThread::downloadImage(QString link)
{
    action = "image";
    curImage = link;
    wdatos->get(webRequest(QUrl(link)));
}

void WebThread::downloadImageExtern(QString link, int index)
{
    action = "imageextern";
    curImage = link;
    curImageIndex = index;
    qDebug() << "downloading extern image" << link;
    wdatos->get(QNetworkRequest(QUrl(link)));
}

QString WebThread::saveToDisk(QIODevice *reply)
{

    QString art = files[0][0];
    QString alb = files[0][1];
    QString th2 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/media-art/album-"+ doubleHash(art, alb) + ".jpeg";
    if (alb.isEmpty())
        th2 = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/artist-" + hash(art) + ".jpeg";

    QImage image = QImage::fromData(reply->readAll());
    if (image.isNull() || !image.save(th2, "JPEG"))
        return "ERROR";

    return th2;
}

QString WebThread::saveToDiskExtern(QIODevice *reply)
{
    QImage image = QImage::fromData(reply->readAll());

    QString path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/flowplayer/" + hash(curImage) + ".jpeg";

    image.save(path, "JPEG");

    return path;
}

