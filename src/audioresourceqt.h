#ifndef LIBAUDIORESOURCEQT_AUDIORESOURCE_QT_H
#define LIBAUDIORESOURCEQT_AUDIORESOURCE_QT_H

#include <QObject>

#if defined(LIBAUDIORESOURCEQT_LIBRARY)
#  define AUDIORESOURCEQT_EXPORT Q_DECL_EXPORT
#else
#  define AUDIORESOURCEQT_EXPORT Q_DECL_IMPORT
#endif


namespace AudioResourceQt {

class AudioResourcePriv;

class AUDIORESOURCEQT_EXPORT AudioResource : public QObject {
    Q_OBJECT
    Q_ENUMS(ResourceType)

public:
    enum ResourceType {
        InvalidType = 0,
        GameType = 1,
        MediaType = 2,
    };

    explicit AudioResource(QObject *parent=0, enum AudioResource::ResourceType type=AudioResource::InvalidType);
    ~AudioResource();

    Q_PROPERTY(bool acquired READ isAcquired NOTIFY acquiredChanged)
    Q_PROPERTY(enum AudioResource::ResourceType resourceType READ resourceType WRITE setResourceType NOTIFY resourceTypeChanged)

    bool isAcquired();
    enum AudioResource::ResourceType resourceType();

signals:
    void acquiredChanged();
    void resourceTypeChanged();

public slots:
    bool acquire();
    bool release();
    void setResourceType(enum AudioResource::ResourceType type);

private:
    AudioResourcePriv *d;

    friend class AudioResourcePriv;
};

}; /* namespace AudioResourceQt */

#endif /* LIBAUDIORESOURCEQT_AUDIORESOURCE_QT_H */
