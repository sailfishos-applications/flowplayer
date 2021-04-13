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

#include "photoitem.h"
#include "photoloader.h"
#include <QDir>
#include <QCryptographicHash>

PhotoLoader& PhotoLoader::instance()
{
    static PhotoLoader singleton;
    return singleton;
}

void PhotoLoader::load(PhotoItem* item)
{

    QString art = item->artist().toLower();
    QString alb = item->album().toLower();

    QCryptographicHash md(QCryptographicHash::Md5);
    QCryptographicHash md2(QCryptographicHash::Md5);

    QByteArray ba = clear(art).toUtf8();
    md.addData(ba);

    QByteArray ba2 = clear(alb).toUtf8();
    md2.addData(ba2);

    QString th1 = "/home/user/.cache/media-art/album-"+ QString(md.result().toHex().constData()) +
                    "-" + QString(md2.result().toHex().constData()) + ".jpeg";

    QImage image;
    if ( QFileInfo(th1).exists() )
        image.load(th1);
    else
        image.load("");

    //item->setImage(image);

    if ( QFileInfo(th1).exists() )
        item->setArt("file://" + th1);
    else
        item->setArt("qrc:/images/nocover.jpg");



}

QString PhotoLoader::clear(QString data)
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
    str.remove(")");
    str.remove("(");
    str.replace("  "," ").replace("  "," ");
    return str.trimmed();
}

PhotoLoader::PhotoLoader() : QObject()
{

}

PhotoLoader::~PhotoLoader()
{

}
