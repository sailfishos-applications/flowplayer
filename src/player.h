#ifndef PLAYER_H
#define PLAYER_H

#include <QQuickItem>
#include <QTimer>

#include <gst/gst.h>

#include "audioresourceqt.h"

class Player : public QQuickItem
{
    Q_OBJECT
public:
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int state READ state NOTIFY stateChanged)
    Q_PROPERTY(double position READ position NOTIFY positionChanged)
    Q_PROPERTY(double duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(bool eqenabled READ eqenabled NOTIFY eqEnabledChanged)

    Player(QQuickItem *parent = 0);


    QString source() { return m_source; }
    int state() { return m_state; }
    int position() { return m_position; }
    int duration() { return m_duration; }
    bool eqenabled() { return m_eqenabled; }

    QString nextSource;
    int prevDuration;

    QTimer *timer;
    QString m_source;
    int m_state;
    int m_position;
    int m_duration;
    bool m_eqenabled;
    QVariantMap eq, preveq;

    gboolean handleBusMessage(GstBus * bus, GstMessage * msg);

public slots:
    void backend_init(int *argc, char **argv[]);
    void backend_deinit();
    void play();
    void stop();
    void pause();
    void resume();
    void seek(int val);
    void backend_seek(gint value);
    void backend_seek_absolute(guint64 value);
    void backend_reset();
    guint64 backend_query_position();
    guint64 backend_query_duration();

    void setSource(QString file, bool stopcurrent=true);
    void setNextSource(QString file);
    void getPosition();
    void getDuration();

    void setEq(bool enabled);
    void setEqualizer(int band, int value);
    void setEqualizerReal(int band, int value);
    void loadEqualizer();
    void loadActualEqualizer();
    void applyEqualizer();

    void onAcquiredChanged();

    void savePreset(QString name, bool current);

signals:
    void sourceChanged();
    void stateChanged();
    void positionChanged();
    void durationChanged();
    void eqEnabledChanged();
    void equalizerChanged(QVariantMap eq);
    void presetSaved();
    void aboutToFinish();
    void setNextSong();

private:
    /*GstElement *pipeline;
    GstElement *audiosink;
    GstSeekFlags seek_flags; // = GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT;*/
    GstElement *bin, *equalizer, *convert, *sink;
    GstPad *pad, *ghost_pad;
    GstBus *bus;
    GstMessage *msg;

    AudioResourceQt::AudioResource m_audio_resource;
};

#endif // PLAYER_H
