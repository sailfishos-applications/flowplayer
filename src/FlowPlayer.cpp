
#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QtQuick>
#include <sailfishapp.h>
#include <QObject>
#include <QTextCodec>

#include "playlistmanager.h"
#include "utils.h"
#include "coversearch.h"
#include "meta.h"
#include "lfm.h"
#include "datos.h"
#include "missing.h"
#include "database.h"
#include "radios.h"
#include "player.h"

bool isDBOpened;
bool databaseWorking;

static void migrateSettings()
{
    const QString oldSettings = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/cepiperez/flowplayer.conf";
    const QString newSettings = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + ".conf";
    if (QFile::exists(oldSettings)) {
        if (QDir().mkpath(QFileInfo(newSettings).path())
            && !QFile::rename(oldSettings, newSettings)) {
            qWarning() << "unable to move old configuration from" << oldSettings << "to" << newSettings;
        }
        QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)).rmdir("cepiperez");
    }
}

static void migrateDatabase()
{
    const QString olderDb = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/cepiperez/flowplayer.db";
    const QString oldDb = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/flowplayer/flowplayer/flowplayer.db";
    const QString newDb = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/flowplayer.db";
    if (QFile::exists(oldDb)) {
        if (QDir().mkpath(QFileInfo(newDb).path())
            && !QFile::rename(oldDb, newDb)) {
            qWarning() << "unable to move old database from" << oldDb << "to" << newDb;
        }
        QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/flowplayer").rmdir("flowplayer");
        QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)).rmdir("flowplayer");
    } else if (QFile::exists(olderDb)) {
        if (QDir().mkpath(QFileInfo(newDb).path())
            && !QFile::rename(olderDb, newDb)) {
            qWarning() << "unable to move old database from" << olderDb << "to" << newDb;
        }
        QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)).rmdir("cepiperez");
    }
}

static void migrateCache()
{
    const QString olderCache = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/flowplayer";
    const QString oldCache = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/flowplayer/flowplayer";
    const QString newCache = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (QFileInfo(oldCache).isDir()) {
        if (QDir().mkpath(QFileInfo(newCache).path())
            && !QDir().rename(oldCache, newCache)) {
            qWarning() << "unable to move old cache from" << oldCache << "to" << newCache;
        }
        QDir(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + "/flowplayer").rmdir("flowplayer");
        QDir(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)).rmdir("flowplayer");
    } else if (QFileInfo(olderCache).isDir()) {
        if (QDir().mkpath(QFileInfo(newCache).path())
            && !QDir().rename(olderCache, newCache)) {
            qWarning() << "unable to move old cache from" << olderCache << "to" << newCache;
        }
        QDir(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)).rmdir("flowplayer");
    }
}

int main(int argc, char *argv[])
{
    QTextCodec *linuxCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(linuxCodec);

    QGuiApplication *app = SailfishApp::application(argc, argv);
    app->setOrganizationName("sailfishos-applications");
    app->setApplicationName("flowplayer");

    migrateSettings();
    migrateDatabase();
    migrateCache();

    QString lang;
    QTranslator translator;

    QSettings settings;
    lang = settings.value("Language", "undefined").toString();

    if (lang=="undefined")
    {
        lang=  QLocale().name();
        qDebug() << "Getting language from locale:" << lang;
    }
    else
    {
        qDebug() << "Stored language: " << lang;
    }

    if (QFile::exists("/usr/share/flowplayer/translations/"+ lang + ".qm"))
        translator.load("/usr/share/flowplayer/translations/" + lang);
    else
        translator.load("/usr/share/flowplayer/translations/en");

    app->installTranslator(&translator);


    QScopedPointer<QQuickView> window(SailfishApp::createView());
    window->setTitle("FlowPlayer");

    window->engine()->addImportPath("/usr/share/flowplayer/qml");
    window->rootContext()->setContextProperty("appVersion", VERSION);

    qmlRegisterType<Utils>("FlowPlayer", 1, 0, "Utils");
    qmlRegisterType<CoverSearch>("FlowPlayer", 1, 0, "CoverSearch");
    qmlRegisterType<Meta>("FlowPlayer", 1, 0, "Meta");
    qmlRegisterType<LFM>("FlowPlayer", 1, 0, "LFM");
    qmlRegisterType<Datos>("FlowPlayer", 1, 0, "Datos");
    qmlRegisterType<PlaylistManager>("FlowPlayer", 1, 0, "MyPlaylistManager");
    qmlRegisterType<Missing>("FlowPlayer", 1, 0, "Missing");
    qmlRegisterType<Database>("FlowPlayer", 1, 0, "Database");
    qmlRegisterType<Radios>("FlowPlayer", 1, 0, "Radios");

    qmlRegisterType<Player>("FlowPlayer", 1, 0, "Player");

    //qmlRegisterType<Playlist>("FlowPlayer", 1, 0, "Playlist");
    //qmlRegisterType<MyPlaylist>("FlowPlayer", 1, 0, "MyPlaylist");
    //qmlRegisterType<MyPlaylists>("FlowPlayer", 1, 0, "MyPlaylists");
    //qmlRegisterType<MusicModel>("FlowPlayer", 1, 0, "MusicModel");

    window->setSource(SailfishApp::pathTo("qml/flowplayer.qml"));

    window->showFullScreen();
    return app->exec();
    qDebug() << "Good bye!";
}

