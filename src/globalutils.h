#ifndef GLOBALUTILS_H
#define GLOBALUTILS_H

#include <QString>
#include <QCryptographicHash>
#include <QRegExp>

QString hash(QString info);
QString doubleHash(QString artist, QString album);
QString clear(QString data);
QString cleanItem(QString data);
QString escapeForDb(QString data);
QString unescapeFromDb(QString data);
QString artistAlbumCover(QString artist);


#endif // GLOBALUTILS_H
