/*
 * Copyright (c) 2011 Petru Motrescu <petru.motrescu@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "photomodelbuilder.h"
#include <QSparqlConnection>
#include <QSparqlQueryModel>
#include <QSparqlQuery>
#include <QSparqlError>
#include <QDBusConnection>
#include <QStringList>
#include <QDebug>

#define SPARQL_SELECT "SELECT ?artist ?album "\
    "WHERE { ?song a nmm:MusicPiece . " \
    "?song nmm:performer ?artist . ?song nmm:musicAlbum ?album . } GROUP BY ?album " \
    "ORDER BY ASC (?artist)"

#define SPARQL_SELECT2 "SELECT ?album ?artist WHERE { ?song nmm:musicAlbum ?album ; " \
    "nmm:performer ?artist . } GROUP BY ?album ?artist ORDER BY ASC(?artist)"

#define SPARQL_SELECT3 "SELECT ?album ?artist COUNT(?artist) as ?acount WHERE { ?album a nmm:MusicAlbum ; " \
    "nmm:albumArtist ?artist . } GROUP BY ?album ORDER BY ASC(?album)"

#define SPARQL_SELECT4 "SELECT ?album ?artist COUNT(distinct ?artist) as acount " \
    "COUNT(distinct ?song) as songs WHERE { ?album a nmm:MusicAlbum ; nmm:albumArtist ?artist . " \
    "?song nmm:musicAlbum ?album } GROUP BY ?album ORDER BY ASC(?album)"

PhotoModelBuilder& PhotoModelBuilder::instance()
{
    static PhotoModelBuilder singleton;
    return singleton;
}

QSparqlQueryModel* PhotoModelBuilder::model()
{
    return m_model;
}

void PhotoModelBuilder::setOrder(QString order)
{
    qDebug() << "Change order " << order;
    if ( order == "0" )
        m_query = SPARQL_SELECT4;
    else
        m_query = SPARQL_SELECT4;

    m_model->setQuery(QSparqlQuery(m_query), *m_connection);
    emit modelChanged();
}

void PhotoModelBuilder::changed(const QString&)
{
    m_model->setQuery(QSparqlQuery(m_query), *m_connection);
}

PhotoModelBuilder::PhotoModelBuilder() : QObject(),
    m_connection(new QSparqlConnection("QTRACKER_DIRECT")),
    m_model(new QSparqlQueryModel()),
    m_query(SPARQL_SELECT4)
{
    /*m_model->setQuery(QSparqlQuery(m_query), *m_connection);

    // Live updates
    const QString service("org.freedesktop.Tracker1");
    const QString resourcesInterface("org.freedesktop.Tracker1.Resources");
    const QString resourcesPath("/org/freedesktop/Tracker1/Resources");
    const QString changedSignal("GraphUpdated");
    const QString changedSignature("sa(iiii)a(iiii)");
    const QString className("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Image");
    QDBusConnection::sessionBus().connect(service, resourcesPath,
                                          resourcesInterface, changedSignal,
                                          QStringList() << className,
                                          changedSignature,
                                          this, SLOT(changed(QString)));*/
}

PhotoModelBuilder::~PhotoModelBuilder()
{
    delete m_model;
    delete m_connection;
}
