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

#ifndef PHOTOTHUMBNAIL_H
#define PHOTOTHUMBNAIL_H

//! These are the settings used by the N9 Gallery application. With these
//! settings your app will take advantage of already generated thumbnails.
//! Should there be a need to generate new grid thumbnails, your app will
//! be the first one to do so, therefore make sure that the size and the
//! extension are never changed for as long as the "grid" folder is used.
//! If you need to change the size or the extension, the folder must be
//! changed as well.
namespace PhotoThumbnail
{
    const QString Folder = "grid";
    const QSize Size(170, 170);
    const QString Extension = "jpeg";
}

#endif // PHOTOTHUMBNAIL_H
