#include "database.h"
#include <QDirIterator>

Database::Database(QQuickItem *parent)
    : QQuickItem(parent)
{
    reader = new DataReader();
    connect (reader, SIGNAL(finished()), this, SLOT(readed()) );
    connect (reader, SIGNAL(percent(int)), this, SLOT(percent(int)) );
    connect (reader, SIGNAL(total(int)), this, SLOT(total(int)) );
    m_loaded = true;
    //emit loadChanged();
}


void Database::readMusic()
{
    m_loaded = false;
    emit loadChanged();

    m_perc = "0";
    p_perc = 0;
    emit percChanged();

    totalfiles = 0;
    reader->openDB();
    reader->start(QThread::NormalPriority);
}

void Database::cancelRead()
{
    reader->forceFinish = true;
}

void Database::readed()
{
    qDebug() << "DONE! ";
    m_loaded = true;
    emit loadChanged();

}

void Database::total(int val)
{
    totalfiles = val;
}

void Database::percent(int val)
{
    int num = val* 100 / totalfiles;
    if (num>p_perc)
    {
        p_perc = num;
        m_perc = QString::number(num);
        emit percChanged();
    }
}
