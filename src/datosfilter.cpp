#include "datosfilter.h"

DatosFilter::DatosFilter(QObject *parent) : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterRole(Qt::UserRole + 1);
    setSortRole(Qt::UserRole + 1);

    m_datos = new Datos();

    setSourceModel(m_datos);

    connect(m_datos, SIGNAL(countChanged()), this, SIGNAL(countChanged()));
    connect(m_datos, SIGNAL(loadChanged()), this, SIGNAL(loadChanged()));

}
