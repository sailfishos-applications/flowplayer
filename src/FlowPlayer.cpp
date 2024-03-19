
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

int main(int argc, char *argv[])
{
    QTextCodec *linuxCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(linuxCodec);

    QGuiApplication *app = SailfishApp::application(argc, argv);
    app->setOrganizationName("sailfishos-applications");
    app->setApplicationName("flowplayer");

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

    // ensure the media cache dir is created
    const QString mediaCacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/media-art";
    QDir().mkpath(mediaCacheDir);

    QScopedPointer<QQuickView> window(SailfishApp::createView());
    window->setTitle("FlowPlayer");

    window->engine()->addImportPath("/usr/share/flowplayer/qml");
    window->rootContext()->setContextProperty("appVersion", VERSION);
    bool hasPickers = false;
    for (const QString &path : window->engine()->importPathList()) {
        hasPickers = hasPickers || QFile::exists(path + "/Sailfish/Pickers");
    }
    window->rootContext()->setContextProperty("hasPickers", hasPickers);

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

