#include "radios.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

bool Radios::radioExists(QString name, QString url)
{
    name = reemplazar1(name);
    url = reemplazar1(url);
    bool res = executeQueryCheckCount(QString("select * from radio where name='%1' and url='%2'").arg(name).arg(url));
    qDebug() << "radio exits " << name << res;
    return res;
}

Radios::Radios(QQuickItem *parent)
    : QQuickItem(parent)
{
    datos = new QNetworkAccessManager(this);
    connect(datos, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded(QNetworkReply*)));

}

QString Radios::reemplazar1(QString data)
{
    data.replace("&","&amp;");
    data.replace("<","&lt;");
    data.replace(">","&gt;");
    data.replace("\"","&quot;");
    data.replace("\'","&apos;");
    return data;
}

QString Radios::reemplazar2(QString data)
{
    data.replace("&amp;", "&");
    data.replace("&lt;", "<");
    data.replace("&gt;", ">");
    data.replace("&quot;", "\"");
    data.replace("&apos;", "\'");
    return data;
}

void Radios::loadRadios()
{
    QSqlQuery query = getQuery(QString("select * from radio order by name"));

    while( query.next() )
    {
        QString dato1, dato2, dato3, dato4;
        dato1 = reemplazar2(query.value(0).toString());
        dato2 = reemplazar2(query.value(1).toString());
        dato3 = reemplazar2(query.value(2).toString());
        dato4 = reemplazar2(query.value(3).toString());

        emit appendRadio(dato1, dato2, dato3, dato4);
    }

}

void Radios::searchRadio(QString text)
{
    action = "search";
    QString url = "http://api.dar.fm/playlist.php?q=@callsign%20" + text.trimmed() + "*&web=1&pagesize=100&callback=?";
    qDebug() << "Searching radio" << text;
    reply = datos->get(QNetworkRequest(QUrl(url)));
}

void Radios::getRadioInfo(QString text)
{
    action = "getinfo";
    QString url = "http://api.dar.fm/player_api.php?station_id=" + text.trimmed() + "&partner_token=1123581337";
    qDebug() << "Getting radio info" << text;
    reply = datos->get(QNetworkRequest(QUrl(url)));
}

void Radios::getPlayingInfo(QString text)
{
    if (text=="")
        return;

    action = "playinfo";

    if (text!=currentRadio) {
        currentArtist = "";
        currentTitle = "";
    }

    currentRadio = text;
    QString url = "http://api.dar.fm/playlist.php?station_id=" + text.trimmed() + "&callback=?";
    qDebug() << "Get playing info for" << text;
    reply = datos->get(QNetworkRequest(QUrl(url)));
}

void Radios::saveRadio(QString name, QString url, QString id, QString image)
{
    qDebug() << "Save radio" << name << url << id << image;
    name = reemplazar1(name);
    url = reemplazar1(url);
    image = reemplazar1(image);

    bool res = executeQueryCheckCount(QString("select from radio where name='%1' and url='%2'")
                                      .arg(name).arg(url));

    if (!res)
        res = executeQuery(QString("insert into radio values('%1','%2','%3','%4')")
                           .arg(name).arg(url).arg(id).arg(image));

    qDebug() << "ADDING " << name << res;

}

void Radios::removeRadio(QString name, QString url)
{
    name = reemplazar1(name);
    url = reemplazar1(url);
    bool res = executeQuery(QString("delete from radio where name='%1' and url='%2'")
                                      .arg(name).arg(url));
    qDebug() << "Removed radio " << name << res;

}

void Radios::downloaded(QNetworkReply *respuesta)
{
    if (respuesta->error() != QNetworkReply::NoError)
    {
        emit radioInfoLoaded("ERROR", "ERROR");
    }
    else
    {
        QString datos1 = respuesta->readAll();
        if (action == "search")
        {
            datos1.replace("?(", "{\"list\": ");
            datos1.chop(1);
            datos1.append("}");

            QJsonDocument jsonResponse = QJsonDocument::fromJson(datos1.toUtf8());

            foreach(const QVariant &itemJson, jsonResponse.object().toVariantMap()["list"].toList())
            {
                QVariantMap itemMap = itemJson.toMap();
                QString name = itemMap["callsign"].toString();
                QString genre = itemMap["genre"].toString();
                QString id = itemMap["station_id"].toString();
                emit appendRadioSearch(name, genre, id);
            }
            emit appendRadioDone();

        }
        else if (action == "getinfo")
        {
            if (datos1.contains("cannot play in a browser"))
            {
                emit radioInfoLoaded("ERROR", "ERROR");
                return;
            }

            qDebug() << datos1;
            QString tmp = datos1;
            int x = tmp.indexOf("var playUrl = ");
            tmp.remove(0,x+15);
            x = tmp.indexOf("\";");
            tmp.remove(x,tmp.length()-x);
            QString radiourl = tmp;

            tmp = datos1;
            x = tmp.indexOf("var stationImage = ");
            tmp.remove(0,x+20);
            x = tmp.indexOf("\";");
            tmp.remove(x,tmp.length()-x);

            emit radioInfoLoaded(radiourl, tmp);
        }
        else if (action == "playinfo")
        {
            qDebug() << datos1;

            datos1.replace("?(", "{\"list\": ");
            datos1.chop(1);
            datos1.append("}");

            QJsonDocument jsonResponse = QJsonDocument::fromJson(datos1.toUtf8());

            bool done = false;
            foreach(const QVariant &itemJson, jsonResponse.object().toVariantMap()["list"].toList())
            {
                QVariantMap itemMap = itemJson.toMap();
                QString name = itemMap["station_id"].toString();
                QString artist = itemMap["artist"].toString();
                QString title = itemMap["title"].toString();
                int seconds_remaining = itemMap["seconds_remaining"].toInt();

                if (name==currentRadio && seconds_remaining>0 && !title.contains("???") && !artist.contains("???")) {
                    currentArtist = artist;
                    currentTitle = title;
                    emit playInfoLoaded(artist, title, seconds_remaining);
                    done = true;
                    break;
                }
            }
            if (!done)
                emit playInfoLoaded(currentArtist, currentTitle, 0);

            //QTimer::singleShot(tmp.trimmed().toInt()*1000, this, SLOT(getPlayingInfo(currentRadio)));
        }

    }
}

