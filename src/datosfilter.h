#ifndef DATOSFILTER_H
#define DATOSFILTER_H

#include "datos.h"
#include <QSortFilterProxyModel>

class DatosFilter : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadChanged)


public:
    explicit DatosFilter(QObject *parent = 0);

    QString filter() const { return filterRegExp().pattern(); }

    int count() { return m_datos->count(); }
    bool loaded() { return m_datos->loaded(); }

    Q_INVOKABLE void init() {
        setSourceModel(m_datos);
    }

signals:
    void countChanged();
    void loadChanged();
    void filterChanged();

public slots:
    void clearList() { m_datos->clearList(); }
    void loadArtists() { m_datos->loadArtists(); }
    void loadAlbums(QString order) { m_datos->loadAlbums(order); }
    void reloadItem(QString albumpath) { m_datos->reloadItem(albumpath); }
    void setFilter(const QString &filter) { setFilterFixedString(filter); Q_EMIT filterChanged();}


private:
    Datos *m_datos;


};

#endif // DATOSFILTER_H
