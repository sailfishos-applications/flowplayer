#ifndef LOADWEBIMAGE_H
#define LOADWEBIMAGE_H

#include <QStringList>
#include <QThread>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBuffer>


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

};

#endif // LOADWEBIMAGE_H
