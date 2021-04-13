#include "audioresourceqt.h"
#include "audioresource.h"

namespace AudioResourceQt {

static audioresource_type_t
atype_from_qttype(enum AudioResource::ResourceType type)
{
    switch (type) {
        case AudioResource::InvalidType:
            return AUDIO_RESOURCE_INVALID;
        case AudioResource::GameType:
            return AUDIO_RESOURCE_GAME;
        case AudioResource::MediaType:
            return AUDIO_RESOURCE_MEDIA;
    }

    return AUDIO_RESOURCE_INVALID;
}

class AudioResourcePriv {
public:
    AudioResourcePriv(AudioResource *p)
        : p(p)
        , type(AudioResource::InvalidType)
        , atype(atype_from_qttype(type))
        , acquired(false)
        , audioresource(0)
    {
    }

    ~AudioResourcePriv()
    {
        if (audioresource) {
            audioresource_free(audioresource);
        }
    }

    void setAcquired(bool acquired);

    static void acquired_cb(audioresource_t *audio_resource,
            bool acquired, void *user_data);

    AudioResource *p;
    enum AudioResource::ResourceType type;
    enum audioresource_type_t atype;
    bool acquired;
    audioresource_t *audioresource;
};

void
AudioResourcePriv::acquired_cb(audioresource_t *audio_resource,
        bool acquired, void *user_data)
{
    AudioResource *resource = static_cast<AudioResource *>(user_data);
    resource->d->setAcquired(acquired);
}

void
AudioResourcePriv::setAcquired(bool acquired)
{
    if (acquired != this->acquired) {
        this->acquired = acquired;
        emit p->acquiredChanged();
    }
}


AudioResource::AudioResource(QObject *parent, enum AudioResource::ResourceType type)
    : QObject(parent)
    , d(new AudioResourcePriv(this))
{
    setResourceType(type);
}

AudioResource::~AudioResource()
{
    delete d;
}

bool
AudioResource::acquire()
{
    if (d->audioresource) {
        audioresource_acquire(d->audioresource);
        return true;
    }

    return false;
}

bool
AudioResource::release()
{
    if (d->audioresource) {
        audioresource_release(d->audioresource);
        return true;
    }

    return false;
}

bool
AudioResource::isAcquired()
{
    return d->acquired;
}

enum AudioResource::ResourceType
AudioResource::resourceType()
{
    return d->type;
}

void
AudioResource::setResourceType(enum AudioResource::ResourceType type)
{
    if (d->type == type && (type == AudioResource::InvalidType || d->audioresource)) {
        // No need to change anything
        return;
    }

    bool oldAcquired = isAcquired();

    if (d->audioresource) {
        audioresource_free(d->audioresource);
        d->setAcquired(false);
    }

    d->type = type;
    d->atype = atype_from_qttype(d->type);

    if (d->atype != AUDIO_RESOURCE_INVALID) {
        d->audioresource = audioresource_init(d->atype,
                AudioResourcePriv::acquired_cb, this);

        if (oldAcquired) {
            // Re-acquire audio resource
            acquire();
        }
    }

    emit resourceTypeChanged();
}

}; /* namespace AudioResourceQt */
