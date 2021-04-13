#ifndef RADIOS_H
#define RADIOS_H

#include <QQuickItem>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

#include "globalutils.h"
#include "mydatabase.h"

class Radios : public QQuickItem
{
    Q_OBJECT

public:
    Q_INVOKABLE bool radioExists(QString name, QString url);

    Radios(QQuickItem *parent = 0);

    QNetworkAccessManager* datos;
    QNetworkReply *reply;

    QString action;
    QString currentRadio, currentArtist, currentTitle;

    QString reemplazar1(QString data);
    QString reemplazar2(QString data);

public slots:
    void loadRadios();
    void searchRadio(QString text);
    void getRadioInfo(QString text);
    void getPlayingInfo(QString text);
    void saveRadio(QString name, QString url, QString id, QString image);
    void removeRadio(QString name, QString url);

private slots:
    void downloaded(QNetworkReply *respuesta);

signals:
    void appendRadio(QString name, QString genre, QString radioid, QString image);
    void appendRadioSearch(QString name, QString genre, QString radioid);
    void appendRadioDone();
    void radioInfoLoaded(QString radiourl, QString image);
    void playInfoLoaded(QString artist, QString title, int remaining);

};

#endif // RADIOS_H
