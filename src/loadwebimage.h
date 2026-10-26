#ifndef LOADWEBIMAGE_H
#define LOADWEBIMAGE_H

#include <QStringList>
#include <QThread>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBuffer>
#include <QElapsedTimer>
#include <QTimer>
#include <QMap>

QNetworkRequest webRequest(const QUrl &url);
QUrl wikidataPhotosUrl(const QStringList &ids);
QMap<QString, QString> wikidataPhotos(const QByteArray &reply);
QString luceneEscape(QString data);

class WebThread : public QThread
{
    Q_OBJECT

public:
    WebThread();
    virtual ~WebThread();

    QList<QStringList> files;
    QImage result;

    QNetworkAccessManager* wdatos;

    QString saveToDisk(QIODevice *reply);
    QString saveToDiskExtern(QIODevice *reply);
    QString action, curImage;
    int curImageIndex;

    bool canceled;


public slots:
    void addFile(QString artist, QString album, int index);
    void downloaded(QNetworkReply *respuesta);
    void downloadImageExtern(QString link, int index);
    void downloadImage(QString link);
    void checkAll();
    void sendMusicBrainzQuery();
    void run();
    void cancel();
    bool checkInternal();

signals:
    void downloadDone();
    void imgLoaded(QString file, int index);

private:
    QBuffer *buffer;
    QByteArray bytes;
    int Request;
    QElapsedTimer lastMusicBrainzQuery;
    QTimer *musicBrainzDelay;
    QString searchAlbum, artistIds, musicBrainzUrl;
    QStringList photoIds;
    QStringList coverLinks;
    int retries;
    bool artistSearched;

    void nextFile();
    void notFound(int current);
    void queryArtist();
    void queryPhoto(const QStringList &ids);
    void queryMusicBrainz();

};

#endif // LOADWEBIMAGE_H
