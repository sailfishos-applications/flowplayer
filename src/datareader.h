#ifndef DATAREADER_H
#define DATAREADER_H

#include <QThread>
#include <QString>
#include <QStringList>

#include "taglib/fileref.h"
#include "taglib/tag.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "mydatabase.h"

class DataReader : public QThread
{
    Q_OBJECT

public:
    DataReader();

    QStringList files;
    QStringList favFiles;
    QString m_title;
    QString m_artist;
    QString m_album;
    QString m_year;
    QString m_duration;
    QString m_tracknum;

    QString reemplazar1(QString data);
    QString reemplazar2(QString data);

    QList<QVariantMap> map;

    bool forceFinish;


private:
    TagLib::FileRef *tagFile;
    TagLib::File* getFileByMimeType(QString file);
    QSqlDatabase db;

public slots:
    void openDB();
    void insertData(QString url, QString artist, QString album,
                    QString title, int year, int tracknum, int duration);
    void saveData();
    void readFile(QString file);
    void addFile(QString file);
    void clearFiles();
    void run();

signals:
    void finished();
    void percent(int);
    void total(int);

};

#endif // DATAREADER_H
