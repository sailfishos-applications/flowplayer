#include "globalutils.h"

#define QT_GALLERY_MEDIA_ART_ILLEGAL_CHARACTERS \
    "\\(.*\\)|\\{.*\\}|\\[.*\\]|<.*>|[\\(\\)_\\{\\}\\[\\]\\!@#$\\^&\\*\\+\\=\\|\\\\/\"'\\?~`]"

QString hash(QString info)
{
    QString inf = info.toLower().remove(QLatin1String(QT_GALLERY_MEDIA_ART_ILLEGAL_CHARACTERS)).simplified();
    QCryptographicHash md(QCryptographicHash::Md5);
    QByteArray s1 = inf.toUtf8();
    md.addData(s1);
    return QString(md.result().toHex().constData());
}

QString doubleHash(QString artist, QString album)
{
    if ( artist.startsWith("urn:artist:") )
        artist.remove("urn:artist:");

    if ( album.startsWith("urn:album:") )
        album.remove("urn:album:");

    QString art = artist.toLower().remove(QLatin1String(QT_GALLERY_MEDIA_ART_ILLEGAL_CHARACTERS)).simplified();
    QString alb = album.toLower().remove(QLatin1String(QT_GALLERY_MEDIA_ART_ILLEGAL_CHARACTERS)).simplified();

    //art.replace("á", "a").replace("é", "e").replace("í", "i").replace("ó", "o").replace("ú", "u").replace("ñ", "n");
    //alb.replace("á", "a").replace("é", "e").replace("í", "i").replace("ó", "o").replace("ú", "u").replace("ñ", "n");

    QCryptographicHash md(QCryptographicHash::Md5);
    QCryptographicHash md2(QCryptographicHash::Md5);

    QByteArray ba = art.toUtf8();
    md.addData(ba);

    QByteArray ba2 = alb.toUtf8();
    md2.addData(ba2);

    QString th1 = QString(md.result().toHex().constData()) + "-" + QString(md2.result().toHex().constData());
    return th1;
}

QString clear(QString data)
{
    QString str = data;
    str.remove("urn:album:");
    str.remove("urn:artist:");
    str.remove("!");
    str.remove("&");
    str.remove("+");
    str.remove("\'");
    str.remove("\/");
    str.replace("<","(");
    str.replace(">",")");
    str.remove(QRegExp("\([\(].*[\)]\)"));
    str.remove("(");
    str.remove(")");
    str.replace("  "," ").replace("  "," ");
    return str.trimmed();
}

QString cleanItem(QString data)
{
    data = data.toLower().replace("&","and");
    int i = data.indexOf("(");
    if ( i > 0 )
    {
        int j = data.indexOf(")");
        if ( j > 0 ) data.remove(i, j);
    }
    data = data.remove(QRegExp("\\W")).remove(" ");
    return data;
}
