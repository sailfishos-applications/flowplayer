#ifndef UTILS_H
#define UTILS_H

#include <QQuickItem>

#include <QBuffer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "globalutils.h"
#include "mydatabase.h"

class Utils : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString lyrics READ lyrics NOTIFY lyricsChanged)
    Q_PROPERTY(bool nolyrics READ nolyrics NOTIFY lyricsChanged)
    Q_PROPERTY(bool lyricsonline READ lyricsonline NOTIFY lyricsChanged)
    Q_PROPERTY(QString viewmode READ viewmode NOTIFY viewmodeChanged)
    Q_PROPERTY(QString paging READ paging NOTIFY pagingChanged)
    Q_PROPERTY(QString order READ order NOTIFY orderChanged)
    Q_PROPERTY(QString lang READ lang NOTIFY langChanged)
    Q_PROPERTY(QString scrobble READ scrobble NOTIFY scrobbleChanged)
    Q_PROPERTY(QString orientation READ orientation NOTIFY orientationChanged)
    Q_PROPERTY(QString updatestart READ updatestart NOTIFY updateChanged)
    Q_PROPERTY(QString autosearch READ autosearch NOTIFY autosearchChanged)
    Q_PROPERTY(QString cleanqueue READ cleanqueue NOTIFY queueChanged)
    Q_PROPERTY(QString workoffline READ workoffline NOTIFY workofflineChanged)

public:
    Utils(QQuickItem *parent = 0);

    Q_INVOKABLE QString version();

    Q_INVOKABLE QString thumbnail(QString artist, QString album, QString count="1");
    Q_INVOKABLE QString thumbnailArtist(QString artist);

    Q_INVOKABLE QString downloadedCover() { return downloadedAlbumArt; }

    Q_INVOKABLE QString bannerMessage() { return banner; }

    Q_INVOKABLE QString showReflection();
    Q_INVOKABLE QString readSettings(QString set, QString val);

    Q_INVOKABLE QString plainLyrics(QString text);

    Q_INVOKABLE int getShuffleTrack(int current);

    Q_INVOKABLE bool isOnline();

    Q_INVOKABLE bool isFav(QString filename);

    QString accents(QString data);
    QNetworkAccessManager* datos;
    QNetworkReply *reply;

    QString lyrics() { return currentLyrics; }
    bool nolyrics() { return m_noLyrics; }
    bool lyricsonline() { return m_lyricsonline; }

    QString viewmode() const;
    QString paging() const;
    QString orientation() const;
    QString scrobble() const;
    QString order() const;
    QString lang() const;
    QString updatestart() const;
    QString autosearch() const;
    QString cleanqueue() const;
    QString workoffline() const;

    QString reemplazar1(QString data);
    QString reemplazar2(QString data);

    QList<int> items;
    int itemstotal;

    //QMetaDataWriterControl *mcontrol;

public slots:
    void getAlbumArt(QString artist, QString album);
    void removePreview();
    void Finished(int requestId, bool error);

    void getLyrics(QString artist, QString song, QString server);
    void saveLyrics(QString artist, QString song, QString lyrics);
    void saveLyrics2(QString artist, QString song, QString lyrics);
    void readLyrics(QString artist, QString song);
    void cancelFetching();

    void setSettings(QString set, QString val);
    void setViewMode(QString val);
    void setPaging(QString val);
    void setScrobble(QString val);
    void setOrder(QString val);
    void setLang(QString val);
    void setUpdateStart(QString val);
    void setAutoSearch(QString val);
    void setCleanQueue(QString val);
    void setWorkOffline(QString val);

    void setOrientation(QString val);

    void createAlbumArt(QString imagepath);
    void removeAlbumArt();

    void getFolders();
    void getFolderItems(QString path);
    void getFolderItemsUp(QString path);
    void addFolderToList(QString path);
    void removeFolder(QString path);

    void setShuffle(int nitems);

    void loadPresets();
    void removePreset(QString name);

    void updateSongDuration(QString source, int duration);
    void favSong(QString filename, bool fav);


private slots:
    void downloaded(QNetworkReply *respuesta);

private:
    QString currentLyrics;
    bool m_noLyrics, m_lyricsonline;
    QString downloadedAlbumArt;
    QString banner;

    QBuffer *buffer;
    QByteArray bytes;
    int Request;

    QSqlDatabase db;

signals:
    void downloadingCover();
    void coverDownloaded();
    void coverSaved();
    void bannerChanged();
    void lyricsChanged();
    void fetchCanceled();
    void viewmodeChanged();
    void pagingChanged();
    void scrobbleChanged();
    void orientationChanged();
    void orderChanged();
    void langChanged();
    void updateChanged();
    void autosearchChanged();
    void queueChanged();
    void workofflineChanged();

    void addFolder(QString name, QString path);
    void appendFile(QString name, QString path, QString icon);
    void appendFilesDone(QString path);
    void foldersChanged();

    void appendPreset(QString name);

};

#endif // UTILS_H
