#ifndef LOADIMAGE_H
#define LOADIMAGE_H

#include <QStringList>
#include <QThread>
#include <QImage>

class Thread : public QThread
{
    Q_OBJECT

public:
    Thread();
    virtual ~Thread();

    QStringList sourcefiles, destfiles;
    QList<int> sourceindex;
    QImage result;

public slots:
    void addFile(QString filename, QString destname, int index);
    void processImage(QString filename, QString destname, int size);
    void run();

signals:
    void finished();
    void sizeFinished(double size);
    void sizeChanged(double size);
    void imgLoaded(QString file, int index);

};

#endif // LOADIMAGE_H
