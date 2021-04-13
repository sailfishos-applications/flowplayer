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

#ifndef PHOTOITEM_H
#define PHOTOITEM_H

#include <QDeclarativeItem>
#include <QImage>
#include <QString>

class QPainter;
class QWidget;
class QStyleOptionGraphicsItem;

class PhotoItem : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QString artist READ artist WRITE setArtist NOTIFY artistChanged)
    Q_PROPERTY(QString album READ album WRITE setAlbum NOTIFY albumChanged)
    Q_PROPERTY(QString art READ art WRITE setArt NOTIFY artChanged)
    Q_PROPERTY(QString acount READ acount WRITE setAcount NOTIFY acountChanged)
    Q_PROPERTY(bool pressed READ pressed WRITE setPressed NOTIFY pressChanged)

public:
    explicit PhotoItem(QDeclarativeItem* parent = 0);
    ~PhotoItem();

    const QString& artist() const;
    const QString& album() const;
    const QString& art() const;
    const QString& acount() const;
    void setArtist(const QString artist);
    void setAlbum(const QString album);
    bool pressed() const;
    void setPressed(const bool pressed);
    void setImage(QImage);
    void setArt(const QString image);
    void setAcount(const QString acount);

signals:
    void artistChanged();
    void albumChanged();
    void artChanged();
    void acountChanged();
    void pressChanged();

private:
    QString m_artist;
    QString m_album;
    QString m_art;
    QString m_acount;
    bool m_pressed;
    QImage m_image;
};

#endif // PHOTOITEM_H
