TARGET = flowplayer

QT += core network sql xml


CONFIG += link_pkgconfig
PKGCONFIG += gstreamer-1.0 libresource libresource-glib

LIBS += -L"$$_PRO_FILE_PWD_/lib" -lmytaglib

INCLUDEPATH += src/taglib \
               src/taglib/taglib \
               src/taglib/taglib/toolkit \
               src/taglib/taglib/ogg \
               src/taglib/taglib/riff \
               src/taglib/taglib/mpeg/id3v1 \
               src/taglib/taglib/mpeg/id3v2 \
               src/taglib/taglib/mpeg/id3v2/frames \
               src/taglib/taglib/flac \
               src/taglib/taglib/mod \
               src/taglib/taglib/xm \

CONFIG += sailfishapp #taglib

SOURCES += src/FlowPlayer.cpp \
    src/utils.cpp \
    src/coversearch.cpp \
    src/meta.cpp \
    src/lfm.cpp \
    src/datos.cpp \
    src/loadimage.cpp \
    src/playlistmanager.cpp \
    src/missing.cpp \
    src/loadwebimage.cpp \
    src/datareader.cpp \
    src/database.cpp \
    src/mydatabase.cpp \
    src/globalutils.cpp \
    src/datosfilter.cpp \
    src/radios.cpp \
    src/player.cpp \
    src/audioresource.cpp \
    src/audioresourceqt.cpp

OTHER_FILES += \
    qml/pages/FirstPage.qml \
    rpm/FlowPlayer.changes.in \
    rpm/FlowPlayer.spec \
    rpm/FlowPlayer.yaml \
    translations/*.ts \
    flowplayer.desktop \
    qml/flowplayer.qml \
    qml/pages/createobject.js \
    qml/pages/dateandtime.js \
    qml/pages/FastScroll.js \
    qml/pages/TextAreaHelper.js \
    qml/pages/AlbumDelegate.qml \
    qml/pages/AlbumMetadata.qml \
    qml/pages/CoverArtList.qml \
    qml/pages/CoverDownload.qml \
    qml/pages/CoverFlow.qml \
    qml/pages/DownloadingListDelegate.qml \
    qml/pages/Equalizer.qml \
    qml/pages/FastScroll.qml \
    qml/pages/FastScrollStyle.qml \
    qml/pages/FileDelegate.qml \
    qml/pages/FullAlbumSearch.qml \
    qml/pages/Header.qml \
    qml/pages/Lyrics.qml \
    qml/pages/MainPage.qml \
    qml/pages/Metadata.qml \
    qml/pages/NewPlaylist.qml \
    qml/pages/NowPlaying.qml \
    qml/pages/PlaylistDelegate.qml \
    qml/pages/Playlists.qml \
    qml/pages/PlaylistsDelegate.qml \
    qml/pages/Settings.qml \
    qml/pages/SongListView.qml \
    qml/pages/PlaylistPage.qml \
    qml/pages/SelectPlaylist.qml \
    qml/pages/CoverPage.qml \
    qml/pages/SelectLanguage.qml \
    qml/pages/TabHeader.qml \
    qml/pages/StartPage.qml \
    qml/pages/AlbumListView.qml \
    qml/pages/SimpleDelegate.qml \
    qml/pages/OnlineRadios.qml \
    qml/pages/SearchRadio.qml \
    qml/pages/AddRadio.qml \
    qml/pages/Banner.qml \
    qml/pages/SongsPage.qml \
    qml/pages/ManageFolders.qml \
    qml/pages/AddFolder.qml \
    qml/pages/AboutPage.qml \
    qml/pages/SelectPreset.qml \
    qml/pages/LastFM.qml \
    qml/pages/EditPreset.qml \
    qml/pages/MyMediaKeys.qml \
    qml/pages/StartDelegate.qml

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
TRANSLATIONS += translations/ca.ts \
                translations/da.ts  \
                translations/de.ts  \
                translations/es.ts  \
                translations/fr.ts  \
                translations/it.ts  \
                translations/nl.ts  \
                translations/ru.ts  \
                translations/sv.ts

HEADERS += \
    src/utils.h \
    src/coversearch.h \
    src/meta.h \
    src/lfm.h \
    src/datos.h \
    src/loadimage.h \
    src/playlistmanager.h \
    src/missing.h \
    src/loadwebimage.h \
    src/datareader.h \
    src/database.h \
    src/mydatabase.h \
    src/globalutils.h \
    src/datosfilter.h \
    src/radios.h \
    src/player.h \
    src/audioresource.h \
    src/audioresourceqt.h

tag.files = lib/libmytaglib.so.1
tag.path = /usr/share/flowplayer/lib

INSTALLS += tag
