#include "lfm.h"
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QNetworkConfigurationManager>
//#include <QtMultimediaKit/QMetaDataWriterControl>
//#include <QTextEdit>
#include <QSettings>
#include <QDateTime>

//#include <meegotouch/MLocale>

QString lang;

LFM::LFM(QQuickItem *parent)
    : QQuickItem(parent)
{
    datos1 = new QNetworkAccessManager(this);
    //datos2 = new QNetworkAccessManager(this);
    //datos3 = new QNetworkAccessManager(this);
    //datos4 = new QNetworkAccessManager(this);
    //datos5 = new QNetworkAccessManager(this);
    //datos6 = new QNetworkAccessManager(this);

    connect(datos1, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded1(QNetworkReply*)));
    //connect(datos2, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded2(QNetworkReply*)));
    //connect(datos3, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded3(QNetworkReply*)));
    //connect(datos4, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded4(QNetworkReply*)));
    //connect(datos5, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded5(QNetworkReply*)));
    //connect(datos6, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded6(QNetworkReply*)));

    QSettings sets("cepiperez", "flowplayer");
    lang = sets.value("LastFMlang", "en").toString();
}

void LFM::downloaded1(QNetworkReply *respuesta)
{
    QString datares;


    if (action=="getbio")
    {
        if (respuesta->error() != QNetworkReply::NoError)
        {
            qDebug() << "error: " << respuesta->error();
            artistInfo = tr("Error fetching artist information");
            artistInfoLarge = "";
            artistImage = "";
            emit bioChanged();
        }
        else
        {
            datares = QString::fromUtf8(respuesta->readAll());

            if ( datares.contains("<lfm status=\"failed\">") )
            {
                artistImage = "";
                artistInfo = tr("The artist could not be found");
            }
            else
            {
                QString tmp = datares;
                int x = tmp.indexOf("<content>");
                tmp.remove(0,x+9);
                x = tmp.indexOf("</content>");
                tmp.remove(x,tmp.length()-x);
                tmp.remove("<![CDATA[");
                x = tmp.indexOf("User-contributed text is available");
                tmp.remove(x,tmp.length()-x);
                artistInfoLarge = tmp.replace("&lt;", "<").replace("&gt;", ">").replace("&quot;", "\"");

                tmp = datares;
                x = tmp.indexOf("<summary>");
                tmp.remove(0,x+9);
                x = tmp.indexOf("</summary>");
                tmp.remove(x,tmp.length()-x);
                tmp.remove("<![CDATA[");
                tmp.remove("]]");
                x = tmp.indexOf("User-contributed text is available");
                tmp.remove(x,tmp.length()-x);
                artistInfo = tmp.replace("&lt;", "<").replace("&gt;", ">").replace("&quot;", "\"");

                tmp = datares;
                x = tmp.indexOf("<image size=\"mega\">");
                tmp.remove(0,x+19);
                x = tmp.indexOf("<");
                tmp.remove(x,tmp.length()-x);
                tmp = tmp.trimmed();
                //artistImage = tmp;

                if (artistImage=="")
                {
                    action = "getbioimage";
                    QString url = tmp;
                    reply1 = datos1->get(QNetworkRequest(QUrl(url)));
                }
            }
            emit bioChanged();
        }
    }
    else if (action=="getbioimage")
    {
        if (respuesta->error() != QNetworkReply::NoError)
        {
            qDebug() << "error: " << respuesta->error();
            artistInfo = tr("Error fetching artist information");
            artistInfoLarge = "";
            artistImage = "";
            emit bioChanged();
        }
        saveImage(currentArtist);
    }

}

void LFM::downloaded2(QNetworkReply *respuesta)
{
    QString datos1;

    if (respuesta->error() != QNetworkReply::NoError)
    {
        qDebug() << "error: " << respuesta->error();
        albumInfo = tr("Error fetching album information");
        albumInfoLarge = "";
        albumInfoImage = "";
        emit albumInfoChanged();
    }
    else
    {
        datos1 = QString::fromUtf8(respuesta->readAll());

        if ( datos1.contains("<lfm status=\"failed\">") )
        {
            albumInfoImage = "";
            albumInfo = tr("The album could not be found");
        }
        else
        {
            QSettings tsettings2("/usr/share/themes/sailfish/meegotouch/constants.ini", QSettings::IniFormat );
            QString color = tsettings2.value("Special Colors for Ambiance support/COLOR_SPECIAL2","#FFFFFF").toString();
            QString tmp = datos1;

            if ( !datos1.contains("<content>") )
            {
                albumInfoLarge = "";
            }
            else
            {
                int x = tmp.indexOf("<content>");
                tmp.remove(0,x+9);
                x = tmp.indexOf("</content>");
                tmp.remove(x,tmp.length()-x);
                tmp.remove("<![CDATA[");
                x = tmp.indexOf("User-contributed text is available");
                tmp.remove(x,tmp.length()-x);
                albumInfoLarge = "<style type='text/css'>a:link {color:" + color + "; text-decoration:none} " \
                                 "a:visited {color:" + color + "; text-decoration:none}</style>" + tmp;
            }

            if ( !datos1.contains("<content>") )
            {
                albumInfo = tr("No album information available");
            }
            else
            {
                tmp = datos1;
                int x = tmp.indexOf("<summary>");
                tmp.remove(0,x+9);
                x = tmp.indexOf("</summary>");
                tmp.remove(x,tmp.length()-x);
                tmp.remove("<![CDATA[");
                tmp.remove("]]");
                x = tmp.indexOf("User-contributed text is available");
                tmp.remove(x,tmp.length()-x);
                albumInfo = "<style type='text/css'>a:link {color:" + color + "; text-decoration:none} " \
                            "a:visited {color:" + color + "; text-decoration:none}</style>" + tmp;
            }

            tmp = datos1;
            int x = tmp.indexOf("<image size=\"extralarge\">");
            tmp.remove(0,x+25);
            x = tmp.indexOf("<");
            tmp.remove(x,tmp.length()-x);
            tmp = tmp.trimmed();
            albumInfoImage = tmp;
        }
        emit albumInfoChanged();
    }
}

void LFM::downloaded3(QNetworkReply *respuesta)
{
    QString datos1;

    if (respuesta->error() != QNetworkReply::NoError)
    {
        qDebug() << "error: " << respuesta->error();
        songInfo = tr("Error fetching track information");
        songInfoLarge = "";
        songInfoImage = "";
        emit songInfoChanged();
    }
    else
    {
        QSettings tsettings2("/usr/share/themes/sailfish/meegotouch/constants.ini", QSettings::IniFormat );
        QString color = tsettings2.value("Special Colors for Ambiance support/COLOR_SPECIAL2","#FFFFFF").toString();
        datos1 = QString::fromUtf8(respuesta->readAll());

        if ( datos1.contains("<lfm status=\"failed\">") )
        {
            songInfoImage = "";
            songInfo = tr("The track could not be found");
        }
        else
        {
            QString tmp = datos1;

            if ( !datos1.contains("<content>") )
            {
                songInfoLarge = "";
            }
            else
            {
                int x = tmp.indexOf("<content>");
                tmp.remove(0,x+9);
                x = tmp.indexOf("</content>");
                tmp.remove(x,tmp.length()-x);
                tmp.remove("<![CDATA[");
                x = tmp.indexOf("User-contributed text is available");
                tmp.remove(x,tmp.length()-x);
                songInfoLarge = "<style type='text/css'>a:link {color:" + color + "; text-decoration:none} " \
                                "a:visited {color:" + color + "; text-decoration:none}</style>" + tmp;
            }

            if ( !datos1.contains("<content>") )
            {
                songInfo = tr("No track information available");
            }
            else
            {
                tmp = datos1;
                int x = tmp.indexOf("<summary>");
                tmp.remove(0,x+9);
                x = tmp.indexOf("</summary>");
                tmp.remove(x,tmp.length()-x);
                tmp.remove("<![CDATA[");
                tmp.remove("]]");
                x = tmp.indexOf("User-contributed text is available");
                tmp.remove(x,tmp.length()-x);
                songInfo = "<style type='text/css'>a:link {color:" + color + "; text-decoration:none} " \
                           "a:visited {color:" + color + "; text-decoration:none}</style>" + tmp;
            }

            tmp = datos1;
            int x = tmp.indexOf("<image size=\"extralarge\">");
            tmp.remove(0,x+25);
            x = tmp.indexOf("<");
            tmp.remove(x,tmp.length()-x);
            tmp = tmp.trimmed();
            songInfoImage = tmp;
        }
        emit songInfoChanged();
    }
}

void LFM::getBio(QString artist)
{
    artistImage = "";
    QString th2 = "/home/nemo/.cache/flowplayer/artist-" + hash(artist.toLower()) + ".jpeg";
    if (QFileInfo(th2).exists())
        artistImage = th2;

    action = "getbio";
    currentArtist = artist;

    //if ( reply1 && reply1->isRunning() )
    //    reply1->abort();
    QSettings sets("cepiperez", "flowplayer");
    lang = sets.value("Language", "en").toString();
    artistInfo = tr("Fetching artist information");
    artistInfoLarge = "";
    //artistImage = "";
    //qDebug() << "Searching lastfm info..." << artist;
    QString url = "http://ws.audioscrobbler.com/2.0/?method=artist.getinfo&api_key=7f338c7458e7d1a9a6204221ff904ba1";
    reply1 = datos1->get(QNetworkRequest(QUrl(url+"&artist="+artist+"&lang="+lang)));

}

QString LFM::offlineBioImg(QString artist)
{
    QString th2 = "/home/nemo/.cache/flowplayer/artist-" + hash(artist.toLower()) + ".jpeg";
    if (QFileInfo(th2).exists())
        return th2;
    else
        return "";
}

void LFM::getAlbumBio(QString artist, QString album)
{
    /*if ( reply2 && reply2->isRunning() )
        reply2->abort();
    QSettings sets("cepiperez", "flowplayer");
    lang = sets.value("LastFMlang", "en").toString();
    albumInfo = tr("Fetching album information");
    albumInfoLarge = "";
    albumInfoImage = "";
    //qDebug() << "Searching lastfm info..." << artist;
    QString url = "http://ws.audioscrobbler.com/2.0/?method=album.getinfo&api_key=7f338c7458e7d1a9a6204221ff904ba1";
    reply2 = datos2->get(QNetworkRequest(QUrl(url+"&artist="+artist+"&album="+album+"&lang="+lang)));
    */
}

void LFM::getSongBio(QString artist, QString song)
{
    /*if ( reply3 && reply3->isRunning() )
        reply3->abort();
    QSettings sets("cepiperez", "flowplayer");
    lang = sets.value("LastFMlang", "en").toString();
    songInfo = tr("Fetching track information");
    songInfoLarge = "";
    songInfoImage = "";
    //qDebug() << "Searching lastfm info..." << song;
    QString url = "http://ws.audioscrobbler.com/2.0/?method=track.getinfo&api_key=7f338c7458e7d1a9a6204221ff904ba1";
    reply3 = datos3->get(QNetworkRequest(QUrl(url+"&artist="+artist+"&track="+song+"&lang="+lang)));
    */
}

void LFM::loginIn(QString username, QString password)
{
    //qDebug() << "login in.. " << username << password;
    /*if ( reply4 && reply4->isRunning() )
        reply4->abort();

    lastfmToken = hash(username.toLower() + hash(password));

    QString sign = "api_key7f338c7458e7d1a9a6204221ff904ba1authToken" + lastfmToken;
    sign = sign + "methodauth.getMobileSessionusername" + username.toLower() + "c9072be243c18d33861b61958760a8ed";
    sign = hash(sign);

    QString url = "http://ws.audioscrobbler.com/2.0/";
    url.append("?method=auth.getMobileSession");
    url.append("&username=" + username.toLower());
    url.append("&authToken=" + lastfmToken);
    url.append("&api_key=7f338c7458e7d1a9a6204221ff904ba1");
    url.append("&api_sig=" + sign);

    reply4 = datos4->get(QNetworkRequest(QUrl(url)));
    */
}

void LFM::downloaded4(QNetworkReply *respuesta)
{
    QString datos1;

    if (respuesta->error() != QNetworkReply::NoError)
    {
        lastfmKey = "error";
        lastfmToken = "";
    }
    else
    {
        datos1 = QString::fromUtf8(respuesta->readAll());
        //qDebug() << datos1;

        if ( !datos1.contains("<key>") )
        {
            lastfmKey = "error";
            lastfmToken = "";
        }
        else
        {
            QString tmp = datos1;
            int x = tmp.indexOf("<name>");
            tmp.remove(0,x+6);
            x = tmp.indexOf("</name>");
            tmp.remove(x,tmp.length()-x);
            lastfmUser = tmp;

            tmp = datos1;
            x = tmp.indexOf("<key>");
            tmp.remove(0,x+5);
            x = tmp.indexOf("</key>");
            tmp.remove(x,tmp.length()-x);
            lastfmKey = tmp;
        }
    }
    emit loginChanged();

}

void LFM::loveSong(QString artist, QString song)
{
    /*action = "love";
    qDebug() << "loving song...";
    if ( reply5 && reply5->isRunning() )
        reply5->abort();

    QString sign = "api_key7f338c7458e7d1a9a6204221ff904ba1artist" + artist + "methodtrack.love";
    sign = sign + "sk" + lastfmKey + "track" + song +"c9072be243c18d33861b61958760a8ed";
    sign = hash(sign);

    QString url = "http://ws.audioscrobbler.com/2.0/";
    QStringList parameters;
    parameters.append("method=track.love");
    parameters.append("artist=" + artist);
    parameters.append("track=" + song);
    parameters.append("api_key=7f338c7458e7d1a9a6204221ff904ba1");
    parameters.append("api_sig=" + sign);
    parameters.append("sk=" + lastfmKey);

    reply5 = datos5->post(QNetworkRequest(QUrl(url)), parameters.join("&").toUtf8());
    */
}

void LFM::unloveSong(QString artist, QString song)
{
    /*action = "unlove";
    qDebug() << "unloving song... ";
    if ( reply5 && reply5->isRunning() )
        reply5->abort();

    QString sign = "api_key7f338c7458e7d1a9a6204221ff904ba1artist" + artist + "methodtrack.unlove";
    sign = sign + "sk" + lastfmKey + "track" + song +"c9072be243c18d33861b61958760a8ed";
    sign = hash(sign);

    QString url = "http://ws.audioscrobbler.com/2.0/";
    QStringList parameters;
    parameters.append("method=track.unlove");
    parameters.append("artist=" + artist);
    parameters.append("track=" + song);
    parameters.append("api_key=7f338c7458e7d1a9a6204221ff904ba1");
    parameters.append("api_sig=" + sign);
    parameters.append("sk=" + lastfmKey);

    reply5 = datos5->post(QNetworkRequest(QUrl(url)), parameters.join("&").toUtf8());
    */
}


void LFM::downloaded5(QNetworkReply *respuesta)
{
    QString datos1;

    if (respuesta->error() != QNetworkReply::NoError)
    {
        loveStatus = "";
        qDebug() << "error: " << respuesta->error();
    }
    else
    {
        datos1 = QString::fromUtf8(respuesta->readAll());

        if ( !datos1.contains("<lfm status=\"ok\">") )
        {
            loveStatus = "";
        }
        else
        {
            if ( action=="love" )
                loveStatus = "loved";
            else
                loveStatus = "";
        }
    }
    emit loveChanged();

}

void LFM::scrobbleSong(QString artist, QString album, QString song)
{
    //qDebug() << "fetching loved song... " << lastfmToken;
    /*if ( reply6 && reply6->isRunning() )
        reply6->abort();

    QDateTime st(QDate(1970,1,1), QTime(0,0,0));
    int t = st.secsTo(QDateTime::currentDateTime().toUTC());

    QString sign = "album" + album + "api_key7f338c7458e7d1a9a6204221ff904ba1artist" + artist + "methodtrack.scrobble";
    sign = sign + "sk" + lastfmKey + "timestamp" + QString::number(t) + "track" + song +"c9072be243c18d33861b61958760a8ed";
    sign = hash(sign);

    QString url = "http://ws.audioscrobbler.com/2.0/";
    QStringList parameters;
    parameters.append("method=track.scrobble");
    parameters.append("album=" + album);
    parameters.append("artist=" + artist);
    parameters.append("track=" + song);
    parameters.append("timestamp=" + QString::number(t));
    parameters.append("api_key=7f338c7458e7d1a9a6204221ff904ba1");
    parameters.append("api_sig=" + sign);
    parameters.append("sk=" + lastfmKey);

    reply6 = datos6->post(QNetworkRequest(QUrl(url)), parameters.join("&").toUtf8());
    */
}


void LFM::downloaded6(QNetworkReply *respuesta)
{
    QString datos1;

    if (respuesta->error() != QNetworkReply::NoError)
    {
        scrobbleStatus = "";
        qDebug() << "error: " << respuesta->readAll();
    }
    else
    {
        datos1 = QString::fromUtf8(respuesta->readAll());

        if ( !datos1.contains("<lfm status=\"ok\">") )
        {
            scrobbleStatus = "";
        }
        else
        {
            scrobbleStatus = "scrobbled";
        }
    }
    emit scrobbleChanged();

}

void LFM::saveImage(QString artist)
{
    QString th2 = "/home/nemo/.cache/flowplayer/artist-" + hash(artist) + ".jpeg";

    QImage image = QImage::fromData(reply1->readAll());
    image.save(th2, "JPEG");

    artistImage = th2;
    emit bioChanged();
    emit bioImageChanged(th2);
}

void LFM::removeImage(QString artist)
{
    QString th2 = "/home/nemo/.cache/flowplayer/artist-" + hash(artist) + ".jpeg";
    if (QFileInfo(th2).exists())
        QFile::remove(th2);

}
