#ifndef DATABASE_H
#define DATABASE_H

#include <QQuickItem>
#include "datareader.h"

class Database : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadChanged)
    Q_PROPERTY(QString perc READ perc NOTIFY percChanged)

public:
    Database(QQuickItem *parent = 0);

    DataReader *reader;
    int totalfiles;

    bool loaded() const { return m_loaded; }
    bool m_loaded;

    QString perc() const { return m_perc; }
    QString m_perc;
    int p_perc;


public slots:
    void readMusic();
    void readed();
    void percent(int val);
    void total(int val);
    void cancelRead();

signals:
    void loadChanged();
    void percChanged();


};

#endif // DATABASE_H
