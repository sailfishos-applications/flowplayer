#include "loadwebimage.h"
#include "utils.h"

#include <QCryptographicHash>
#include <QUrl>
#include <qstring.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <QImage>
#include <QSettings>
#include <QDebug>
#include <QDateTime>
#include <QStandardPaths>


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

WebThread::WebThread()
{
    wdatos = new QNetworkAccessManager(this);
    connect(wdatos, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded(QNetworkReply*)));

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

    QSqlQuery query = getQuery(QString("select url from tracks where artist='%1' and album='%2' limit 1").arg(artist).arg(album));
    while( query.next() )
    {
        QString dir = QFileInfo(query.value(0).toString()).path();

        if (QFileInfo(dir + "/folder.jpg").exists())
        {
            QString th2 = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/media-art/album-"+ doubleHash(artist, album) + ".jpeg";
            QImage image(dir + "/folder.jpg");
            image.save(th2, "JPEG");
            emit imgLoaded(th2, files[0][2].toInt());
            res = true;
        }

        else if (QFileInfo(dir + "/folder.jpeg").exists())
        {
            QString th2 = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/media-art/album-"+ doubleHash(artist, album) + ".jpeg";
            QImage image(dir + "/folder.jpeg");
            image.save(th2, "JPEG");
            emit imgLoaded(th2, files[0][2].toInt());
            res = true;
        }

        else if (QFileInfo(dir + "/cover.jpg").exists())
        {
            QString th2 = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/media-art/album-"+ doubleHash(artist, album) + ".jpeg";
            QImage image(dir + "/cover.jpg");
            image.save(th2, "JPEG");
            emit imgLoaded(th2, files[0][2].toInt());
            res = true;
        }

        if (QFileInfo(dir + "/Folder.jpg").exists())
        {
            QString th2 = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/media-art/album-"+ doubleHash(artist, album) + ".jpeg";
            QImage image(dir + "/Folder.jpg");
            image.save(th2, "JPEG");
            emit imgLoaded(th2, files[0][2].toInt());
            res = true;
        }

        else if (QFileInfo(dir + "/Folder.jpeg").exists())
        {
            QString th2 = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/media-art/album-"+ doubleHash(artist, album) + ".jpeg";
            QImage image(dir + "/Folder.jpeg");
            image.save(th2, "JPEG");
            emit imgLoaded(th2, files[0][2].toInt());
            res = true;
        }

        else if (QFileInfo(dir + "/Cover.jpg").exists())
        {
            QString th2 = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/media-art/album-"+ doubleHash(artist, album) + ".jpeg";
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
    if (checkInternal())
    {
        files.removeAt(0);

        if (files.count()>0)
            checkAll();
        else
            emit downloadDone();

        return;
    }

    QString artist = files[0][0];
    QString album = files[0][1];


    // LAST.FM
    //QString url = "http://ws.audioscrobbler.com/2.0/?method=album.getinfo&api_key=7f338c7458e7d1a9a6204221ff904ba1";

    // QUASAR
    QString url = "https://coverart.katastrophos.net/query.php?";
    url += "&artist="+QUrl::toPercentEncoding(artist)+"&album="+QUrl::toPercentEncoding(album)+"&mode=imageurls&limit=1";

 
    //AMAZON - SIMPLE
    //QString url = "http://www.amazon.com/gp/search?search-alias=popular";
    //url += "&field-artist="+QUrl::toPercentEncoding(artist)+"&field-title="+QUrl::toPercentEncoding(album)+"&sort=relevancerank";

    action = "link";
    wdatos->get(QNetworkRequest(QUrl(url)));

}

void WebThread::downloaded(QNetworkReply *respuesta)
{
    if (canceled)
        return;

    QString datos1;

    int current = 0;

    if (files.count()>0)
        current = files[0][2].toInt();
    else
        current = curImageIndex;

    if (respuesta->error() != QNetworkReply::NoError)
    {
        if (files.count()>0) {
            current = files[0][2].toInt();
            qDebug() << "error: " << files[0][0] << files[0][1] << respuesta->error();
        }
        emit imgLoaded("ERROR", current);

        if (files.count()>0)
            files.removeAt(0);

        if (files.count()>0)
            checkAll();
        else
            emit downloadDone();
    }
    else
    {

        if (action == "link")
        {
            datos1 = QString::fromUtf8(respuesta->readAll());

            QString tmp = datos1;

            //QUASAR
            if (tmp.startsWith("http"))
            {
                qDebug() << "Link for" << files[0][0] << files[0][1] << tmp;

                downloadImage(tmp);
                //emit imgLoaded(tmp, current);
            }
            else
            {
                emit imgLoaded("ERROR", current);

                qDebug() << "Not found: " << files[0][0] << files[0][1];

                files.removeAt(0);

                if (files.count()>0)
                {
                    checkAll();
                }
                else
                    emit downloadDone();
            }

        }
        else if (action == "image")
        {
            qDebug() << "Saving image for" << files[0][0] << files[0][1];

            QString tmp = saveToDisk(respuesta);

            emit imgLoaded(tmp, current);

            files.removeAt(0);

            if (files.count()>0)
            {
                checkAll();
            }
            else
                emit downloadDone();
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
    files.clear();
}

void WebThread::downloadImage(QString link)
{
    action = "image";
    curImage = link;
    wdatos->get(QNetworkRequest(QUrl(link)));
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
    QString th2 = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/media-art/album-"+ doubleHash(art, alb) + ".jpeg";

    QImage image = QImage::fromData(reply->readAll());
    image.save(th2, "JPEG");

    return th2;
}

QString WebThread::saveToDiskExtern(QIODevice *reply)
{
    QImage image = QImage::fromData(reply->readAll());

    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/flowplayer/" + hash(curImage) + ".jpeg";

    image.save(path, "JPEG");

    return path;
}

