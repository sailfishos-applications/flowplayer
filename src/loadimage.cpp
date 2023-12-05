#include "loadimage.h"
#include "utils.h"

#include <QUrl>
#include <qstring.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <QImage>
#include <QSettings>
#include <QDebug>
#include <QStandardPaths>

Thread::Thread()
{

}

Thread::~Thread()
{

}

void Thread::addFile(QString filename, QString destname, int index)
{
    sourcefiles.append(filename);
    destfiles.append(destname);
    sourceindex.append(index);
}

void Thread::run()
{
    qDebug() << "Starting thread...";
    for (int i=0; i<sourcefiles.count(); ++i)
    {
        processImage(sourcefiles.at(i), destfiles.at(i), 62);
        processImage(sourcefiles.at(i), destfiles.at(i), 170);
        processImage(sourcefiles.at(i), destfiles.at(i), 256);
        emit imgLoaded(destfiles.at(i), sourceindex.at(i));
    }
    emit finished();
    qDebug() << "Thread finished...";

}

void Thread::processImage(QString filename, QString destname, int size)
{
    QImage img(filename);
    if ( img.height() > img.width() ) {
        result = img.scaledToHeight(size,Qt::SmoothTransformation);
        //result = result.copy(0,result.height()/2-32,size,size);
    }
    else if ( img.height() < img.width() )
    {
        result = img.scaledToWidth(size,Qt::SmoothTransformation);
        //result = result.copy(result.width()/2-32,0,size,size);
    }
    else if ( img.height() > size )
    {
        result = img.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        result = result.copy(0,0,size,size);
    }
    else
    {
        result = img;
    }
    result.save(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/" + QString::number(size) + "/album-"+ destname + ".jpeg", "JPEG");

}
