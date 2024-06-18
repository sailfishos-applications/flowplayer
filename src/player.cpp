#include "player.h"
#include "mydatabase.h"

#include <QTimer>
#include <QSettings>
#include <QStandardPaths>
#include "QVariantMap"

extern bool databaseWorking;
extern bool isDBOpened;

static GstElement *pipeline;
//static int got_reply = 0;

static gboolean bus_cb (GstBus *bus, GstMessage *msg, gpointer data)
{
    Player *self = (Player*) data;
    return self->handleBusMessage(bus, msg);
}

static void prepare_next_stream(GstElement *obj, gpointer data) {
    qDebug() << "ABOUT TO FINISH";

    QSettings sets(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/flowplayer.conf", QSettings::NativeFormat);
    if (sets.value("GaplessPlayback", "no").toString()=="no")
        return;

    Player *self = (Player*) data;
    //self->prevSource = self->m_source;
    //emit self->aboutToFinish();

    self->m_source = self->nextSource;
    QByteArray ba = self->m_source.toUtf8();
    gchar *filename = ba.data();

    gchar *uri;

    if (gst_uri_is_valid (filename))
    {
        uri = g_strdup (filename);
    }
    else if (g_path_is_absolute (filename))
    {
        uri = g_filename_to_uri (filename, NULL, NULL);
    }
    else
    {
        gchar *tmp;
        tmp = g_build_filename (g_get_current_dir (), filename, NULL);
        uri = g_filename_to_uri (tmp, NULL, NULL);
        g_free (tmp);
    }

    g_object_set (G_OBJECT (pipeline), "uri", uri, NULL);
    g_free (uri);
    self->nextSource = "";
    self->prevDuration = self->m_duration;

}

static void audio_changed(GstElement * playbin, GstElement *obj, gpointer data) {
    qDebug() << "AUDIO CHANGED";
    Player *self = (Player*) data;

    emit self->sourceChanged();

    self->m_duration = -1;
    self->m_position = 0;
    emit self->durationChanged();
    emit self->positionChanged();
    QMetaObject::invokeMethod(self->timer, "start", Qt::QueuedConnection,
                              Q_ARG(int, 1000));
}

gboolean Player::handleBusMessage (GstBus *, GstMessage *msg)
{
    switch (GST_MESSAGE_TYPE (msg))
    {
        case GST_MESSAGE_EOS:
        {
            qDebug() << "End of media";
            m_duration = -1;
            m_position = 0;
            emit durationChanged();
            emit positionChanged();
            m_state = 0;
            emit stateChanged();
            timer->stop();
            break;
        }
        /*case GST_MESSAGE_DURATION_CHANGED:
        {
            qDebug() << "Duration changed";
            getDuration();
            break;
        }*/
        case GST_MESSAGE_ERROR:
        {
            gchar *debug;
            GError *err;

            gst_message_parse_error (msg, &err, &debug);
            g_free (debug);

            g_warning ("Error: %s", err->message);
            g_error_free (err);

            stop();
            break;
        }
        default:
            break;
    }

    return TRUE;
}


Player::Player(QQuickItem *parent)
    : QQuickItem(parent),
    m_audio_resource(this, AudioResourceQt::AudioResource::MediaType)
{
    equalizer = NULL;
    backend_init(NULL,NULL);
    //pipeline = NULL;
    m_state = 0;
    emit stateChanged();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(getPosition()));
    timer->setInterval(1000);
}

void Player::backend_init (int *argc,
              char **argv[])
{
    gst_init (argc, argv);
    //void *user_data = NULL;

    QObject::connect(&m_audio_resource, SIGNAL(acquiredChanged()),
                         this, SLOT(onAcquiredChanged()));

    loadEqualizer();

    pipeline = gst_element_factory_make ("playbin3", "player");

    equalizer = gst_element_factory_make ("equalizer-10bands", "equalizer");
    convert = gst_element_factory_make ("audioconvert", "convert");
    sink = gst_element_factory_make ("autoaudiosink", "audio_sink");
    if (!equalizer || !convert || !sink) {
        qDebug() << "Not all elements could be created!!!";
        return;
    }

    /* Create the sink bin, add the elements and link them */
    bin = gst_bin_new ("audio_sink_bin");
    gst_bin_add_many (GST_BIN (bin), equalizer, convert, sink, NULL);
    gst_element_link_many (equalizer, convert, sink, NULL);
    pad = gst_element_get_static_pad (equalizer, "sink");
    ghost_pad = gst_ghost_pad_new ("sink", pad);
    gst_pad_set_active (ghost_pad, TRUE);
    gst_element_add_pad (bin, ghost_pad);
    gst_object_unref (pad);

    /* Set playbin2's audio sink to be our sink bin */
    g_object_set (GST_OBJECT (pipeline), "audio-sink", bin, NULL);

    g_signal_connect(pipeline, "about-to-finish",
            G_CALLBACK(prepare_next_stream), this);

    g_signal_connect(pipeline, "source-setup",
            G_CALLBACK(audio_changed), this);

    GstBus *bus;
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    gst_bus_add_watch (bus, bus_cb, this);
    gst_object_unref (bus);

    m_audio_resource.acquire();
}

void Player::onAcquiredChanged()
{
    bool acquired = m_audio_resource.isAcquired();

    if (acquired)
    {
        qDebug() << "Acquired... Playing";

        //gst_element_set_state (pipeline, GST_STATE_PLAYING);

        //m_state = 1;
        //emit stateChanged();

        //loadEqualizer();
        applyEqualizer();

        //timer->start();
    }
    else if (m_state==1)
    {
        qDebug() << "NOT Acquired... STOPPED";
        m_audio_resource.release();
        m_audio_resource.acquire();
        pause();
    }


}

void Player::setNextSource(QString file)
{
    qDebug() << "NEXT Source: " << file;
    nextSource = file;
}

void Player::setSource(QString file, bool stopcurrent)
{
    if (stopcurrent)
        stop();

    //gst_element_set_state (pipeline, GST_STATE_NULL);

    m_source = file;
    qDebug() << "Set source: " << m_source;

    QByteArray ba = m_source.toUtf8();
    gchar *filename = ba.data();

    gchar *uri;

    if (gst_uri_is_valid (filename))
    {
        uri = g_strdup (filename);
    }
    else if (g_path_is_absolute (filename))
    {
        uri = g_filename_to_uri (filename, NULL, NULL);
    }
    else
    {
        gchar *tmp;
        tmp = g_build_filename (g_get_current_dir (), filename, NULL);
        uri = g_filename_to_uri (tmp, NULL, NULL);
        g_free (tmp);
    }

    g_object_set (G_OBJECT (pipeline), "uri", uri, NULL);
    g_free (uri);

    /*gchar *name;
    g_object_get (G_OBJECT (pipeline), "uri", &name, NULL);
    g_print ("The name of the element is '%s'.\n", name);
    g_free (name);*/

    //emit sourceChanged();

}


void Player::play()
{
    //qDebug() << "file: " << m_source;
    //stop();
    prevDuration = 0;
    m_duration = -1;
    m_position = 0;
    emit positionChanged();
    emit durationChanged();

    //m_audio_resource.acquire();
    //emit sourceChanged();
    resume();

}

void Player::getDuration()
{
    int duration = backend_query_duration() / 1000000 /1000;
    qDebug() << "Duration" << duration << "::" << m_duration << " - prev:" << prevDuration;
    if ((m_duration!=duration || m_duration==-1) && duration!=prevDuration) {
        prevDuration = 0;
        m_duration = duration;
        emit durationChanged();
    }
}

void Player::getPosition()
{
    if (nextSource=="")
        return;

    m_position = backend_query_position() / 1000000 /1000;
    //qDebug() << "Position" << m_position << " :: " << m_duration;

    if (m_position<=m_duration)
        emit positionChanged();

    if (m_duration==-1 || m_duration==prevDuration)
        getDuration();

    /*if (m_position==0) {
        qDebug() << "Stopping timer";
        timer->stop();
    }*/

}

void Player::stop()
{
    if (pipeline)
    {
        gst_element_set_state (pipeline, GST_STATE_NULL);
        //gst_object_unref (GST_OBJECT (pipeline));
        //pipeline = NULL;
    }
    m_position = 0;
    emit positionChanged();
    m_duration = -1;
    emit durationChanged();
    m_state = -1;
    emit stateChanged();
    timer->stop();

    //m_audio_resource.release();
}

void Player::pause()
{
    gst_element_set_state (pipeline, GST_STATE_PAUSED);
    m_state = 2;
    emit stateChanged();
    timer->stop();
    //m_audio_resource.release();
}

void Player::resume()
{
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    m_state = 1;
    emit stateChanged();
    timer->start(1000);
    //m_audio_resource.acquire();
}

void Player::seek(int val)
{
    gint64 position = val; // /1000;

    qDebug() << "TOTAL:" << m_duration << " - current:" << m_position << " :: " << val << position;

    bool isSeeking = gst_element_seek(pipeline,
                                      1.0,
                                      GST_FORMAT_TIME,
                                      GstSeekFlags(GST_SEEK_FLAG_FLUSH),
                                      GST_SEEK_TYPE_SET,
                                      position * GST_SECOND,
                                      GST_SEEK_TYPE_NONE,
                                      0);
    if (isSeeking) {
        m_position = val;
        emit positionChanged();
    }
}

void Player::backend_reset()
{
    gst_element_seek (pipeline, 1.0,
                      GST_FORMAT_TIME,
                      (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE),
                      GST_SEEK_TYPE_SET, 0,
                      GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

void Player::backend_seek(gint value)
{
    gst_element_seek (pipeline, 1.0,
                      GST_FORMAT_TIME,
                      (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE),
                      GST_SEEK_TYPE_SET, (m_position + value) * GST_SECOND,
                      GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

void Player::backend_seek_absolute(guint64 value)
{
    gst_element_seek (pipeline, 1.0,
                      GST_FORMAT_TIME,
                      (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE),
                      GST_SEEK_TYPE_SET, value,
                      GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

guint64 Player::backend_query_position()
{
    GstFormat format = GST_FORMAT_TIME;
    gint64 cur;
    gboolean result;

    result = gst_element_query_position (pipeline, format, &cur);
    if (!result)
        return GST_CLOCK_TIME_NONE;

    return cur;
}

guint64 Player::backend_query_duration()
{
    GstFormat format = GST_FORMAT_TIME;
    gint64 cur;
    gboolean result;

    result = gst_element_query_duration (pipeline, format, &cur);
    if (!result)
        return GST_CLOCK_TIME_NONE;

    return cur;
}

void Player::backend_deinit()
{
}

void Player::setEq(bool enabled)
{
    qDebug() << "Setting eq: " << enabled;
    QSettings sets(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/flowplayer.conf", QSettings::NativeFormat);
    sets.setValue("Equalizer", enabled? "Yes" : "No");
    sets.sync();

    m_eqenabled = enabled;
    emit eqEnabledChanged();

    if (enabled)
    {
        eq = preveq;
        applyEqualizer();
    }
    else
    {
        preveq = eq;
        setEqualizer(1, 0);
        setEqualizer(2, 0);
        setEqualizer(3, 0);
        setEqualizer(4, 0);
        setEqualizer(5, 0);
        setEqualizer(6, 0);
        setEqualizer(7, 0);
        setEqualizer(8, 0);
        setEqualizer(9, 0);
        setEqualizer(10, 0);
    }
}

void Player::setEqualizer(int band, int value)
{
    QString b = "band" + QString::number(band);
    eq.remove(b);
    eq.insert(b, value);

    setEqualizerReal(band, value);
}

void Player::setEqualizerReal(int band, int value)
{
    if (!pipeline || pipeline==NULL || !equalizer || equalizer==NULL)
        return;

    QString b = "band" + QString::number(band-1);
    QByteArray ba = b.toLatin1();
    gchar *ban = ba.data();

    g_object_set (G_OBJECT (equalizer), ban, (gdouble)value, NULL);
}

void Player::loadEqualizer()
{
    QSettings sets(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/flowplayer.conf", QSettings::NativeFormat);
    m_eqenabled = sets.value("Equalizer", "No").toString()=="Yes";
    emit eqEnabledChanged();

    QString name = sets.value("Preset", "").toString();

    eq.clear();
    eq.insert("name", name);

    qDebug() << "Loading preset: " << name;

    if (name!="") {
        if (!isDBOpened) openDatabase();
        QSqlQuery query = getQuery(QString("select * from presets where name='%1'").arg(name));

        while( query.next() )
        {
            eq.insert("band1", query.value(1).toDouble());
            eq.insert("band2", query.value(2).toDouble());
            eq.insert("band3", query.value(3).toDouble());
            eq.insert("band4", query.value(4).toDouble());
            eq.insert("band5", query.value(5).toDouble());
            eq.insert("band6", query.value(6).toDouble());
            eq.insert("band7", query.value(7).toDouble());
            eq.insert("band8", query.value(8).toDouble());
            eq.insert("band9", query.value(9).toDouble());
            eq.insert("band10", query.value(10).toDouble());
        }
    }
    else
    {
        eq.insert("band1", 0);
        eq.insert("band2", 0);
        eq.insert("band3", 0);
        eq.insert("band4", 0);
        eq.insert("band5", 0);
        eq.insert("band6", 0);
        eq.insert("band7", 0);
        eq.insert("band8", 0);
        eq.insert("band9", 0);
        eq.insert("band10", 0);

    }

    emit equalizerChanged(eq);

}

void Player::loadActualEqualizer()
{
    emit equalizerChanged(eq);
}

void Player::applyEqualizer()
{
    if (!pipeline || pipeline==NULL || !equalizer || equalizer==NULL)
        return;

    setEqualizerReal(1, eq.value("band1", 0).toDouble());
    setEqualizerReal(2, eq.value("band2", 0).toDouble());
    setEqualizerReal(3, eq.value("band3", 0).toDouble());
    setEqualizerReal(4, eq.value("band4", 0).toDouble());
    setEqualizerReal(5, eq.value("band5", 0).toDouble());
    setEqualizerReal(6, eq.value("band6", 0).toDouble());
    setEqualizerReal(7, eq.value("band7", 0).toDouble());
    setEqualizerReal(8, eq.value("band8", 0).toDouble());
    setEqualizerReal(9, eq.value("band9", 0).toDouble());
    setEqualizerReal(10, eq.value("band10", 0).toDouble());
}

void Player::savePreset(QString name, bool current)
{
    name = name.trimmed();
    preveq = eq;
    if (!current)
    {
        if (!isDBOpened) openDatabase();
        QSqlQuery query = getQuery(QString("select * from presets where name='%1'").arg(name));

        while( query.next() )
        {
            eq.insert("band1", query.value(1).toDouble());
            eq.insert("band2", query.value(2).toDouble());
            eq.insert("band3", query.value(3).toDouble());
            eq.insert("band4", query.value(4).toDouble());
            eq.insert("band5", query.value(5).toDouble());
            eq.insert("band6", query.value(6).toDouble());
            eq.insert("band7", query.value(7).toDouble());
            eq.insert("band8", query.value(8).toDouble());
            eq.insert("band9", query.value(9).toDouble());
            eq.insert("band10", query.value(10).toDouble());
        }
    }

    executeQuery(QString("delete from presets where name='%1'").arg(name));

    QString qr = QString("insert into presets values('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11')")
            .arg(name)
            .arg(eq.value("band1", "0").toString()).arg(eq.value("band2", "0").toString())
            .arg(eq.value("band3", "0").toString()).arg(eq.value("band4", "0").toString())
            .arg(eq.value("band5", "0").toString()).arg(eq.value("band6", "0").toString())
            .arg(eq.value("band7", "0").toString()).arg(eq.value("band8", "0").toString())
            .arg(eq.value("band9", "0").toString()).arg(eq.value("band10", "0").toString());

    executeQuery(qr);

    eq = preveq;

    emit presetSaved();

}
