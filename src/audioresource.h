#ifndef LIBAUDIORESOURCE_AUDIORESOURCE_H
#define LIBAUDIORESOURCE_AUDIORESOURCE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque pointer to our audio resource
typedef struct audioresource_t audioresource_t;

typedef void (*audioresource_acquired_callback_t)(audioresource_t *audio_resource,
        bool acquired, void *user_data);

enum audioresource_type_t {
    AUDIO_RESOURCE_INVALID = 0,
    AUDIO_RESOURCE_GAME = 1,
    AUDIO_RESOURCE_MEDIA = 2,
};

audioresource_t *audioresource_init(enum audioresource_type_t type,
        audioresource_acquired_callback_t acquired_cb,
        void *user_data);

void audioresource_acquire(audioresource_t *audio_resource);

void audioresource_release(audioresource_t *audio_resource);

void audioresource_free(audioresource_t *audio_resource);

#ifdef __cplusplus
};
#endif

#endif /* LIBAUDIORESOURCE_AUDIORESOURCE_H */
