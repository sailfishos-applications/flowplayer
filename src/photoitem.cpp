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
#include <QString>
#include <QStyleOptionGraphicsItem>

static const int Width = 160;
static const int Height = 160;

PhotoItem::PhotoItem(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setImplicitWidth(Width);
    setImplicitHeight(Height);
    setWidth(Width);
    setHeight(Height);
}

PhotoItem::~PhotoItem()
{

}

const QString& PhotoItem::art() const
{
    return m_art;
}

const QString& PhotoItem::artist() const
{
    return m_artist;
}

const QString& PhotoItem::album() const
{
    return m_album;
}

const QString& PhotoItem::acount() const
{
    return m_acount;
}


void PhotoItem::setArtist(const QString artist)
{
    if (m_artist != artist) {
        m_artist = artist;
        emit artistChanged();
        if (!m_artist.isEmpty()) {
            PhotoLoader::instance().load(this);
        }
    }
}

void PhotoItem::setAlbum(const QString album)
{
    if (m_album != album) {
        m_album = album;
        emit albumChanged();
        if (!m_album.isEmpty()) {
            PhotoLoader::instance().load(this);
        }
    }
}

bool PhotoItem::pressed() const
{
    return m_pressed;
}

void PhotoItem::setPressed(const bool pressed)
{
    if (pressed != m_pressed) {
        m_pressed = pressed;
        //update();
        emit pressChanged();
    }
}

void PhotoItem::setImage(QImage image)
{
    m_image = image;
}

void PhotoItem::setArt(const QString image)
{
    m_art = image;
    emit artChanged();
}

void PhotoItem::setAcount(const QString acount)
{
    m_acount = acount;
    emit acountChanged();
}

