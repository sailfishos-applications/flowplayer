#ifndef LFM_H
#define LFM_H

#include <QQuickItem>

#include <QBuffer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

#include "globalutils.h"

class LFM : public QQuickItem
{
    Q_OBJECT

public:
    LFM(QQuickItem *parent = 0);

    Q_INVOKABLE QString offlineBioImg(QString artist);

    Q_INVOKABLE QString bioImg() { return artistImage; }
    Q_INVOKABLE QString bioInfo() { return artistInfo; }
    Q_INVOKABLE QString bioInfoLarge() { return artistInfoLarge; }
    Q_INVOKABLE QString bioImgAlbum() { return albumInfoImage; }
    Q_INVOKABLE QString bioInfoAlbum() { return albumInfo; }
    Q_INVOKABLE QString bioInfoAlbumLarge() { return albumInfoLarge; }
    Q_INVOKABLE QString bioImgSong() { return songInfoImage; }
    Q_INVOKABLE QString bioInfoSong() { return songInfo; }
    Q_INVOKABLE QString bioInfoSongLarge() { return songInfoLarge; }

    Q_INVOKABLE QString lfmUser() { return lastfmUser; }
    Q_INVOKABLE QString lfmKey() { return lastfmKey; }
    Q_INVOKABLE QString lfmLove() { return loveStatus; }
    Q_INVOKABLE QString lfmScrobble() { return scrobbleStatus; }

    QNetworkAccessManager* datos1;
    //QNetworkAccessManager* datos2;
    //QNetworkAccessManager* datos3;
    //QNetworkAccessManager* datos4;
    //QNetworkAccessManager* datos5;
    //QNetworkAccessManager* datos6;
    QNetworkReply *reply1;
    //QNetworkReply *reply2;
    //QNetworkReply *reply3;
    //QNetworkReply *reply4;
    //QNetworkReply *reply5;
    //QNetworkReply *reply6;

    QString action;
    QString currentArtist;


public slots:
    void getBio(QString artist);
    void getAlbumBio(QString artist, QString album);
    void getSongBio(QString artist, QString song);

    void loginIn(QString username, QString password);
    void loveSong(QString artist, QString song);
    void unloveSong(QString artist, QString song);
    void scrobbleSong(QString artist, QString album, QString song);

    void saveImage(QString artist);
    void removeImage(QString artist);

private slots:
    void downloaded1(QNetworkReply *respuesta);
    void downloaded2(QNetworkReply *respuesta);
    void downloaded3(QNetworkReply *respuesta);
    void downloaded4(QNetworkReply *respuesta);
    void downloaded5(QNetworkReply *respuesta);
    void downloaded6(QNetworkReply *respuesta);

private:
    QString artistInfo, artistInfoLarge, artistImage;
    QString albumInfo, albumInfoLarge, albumInfoImage;
    QString songInfo, songInfoLarge, songInfoImage;
    QString lastfmKey, lastfmToken, loveStatus, lastfmUser, scrobbleStatus;

signals:
    void bioChanged();
    void bioImageChanged(QString imagepath);
    void albumInfoChanged();
    void songInfoChanged();
    void loginChanged();
    void loveChanged();
    void scrobbleChanged();
};

#endif // LFM_H
