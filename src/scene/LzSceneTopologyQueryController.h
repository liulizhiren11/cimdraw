#ifndef LZSCENETOPOLOGYQUERYCONTROLLER_H
#define LZSCENETOPOLOGYQUERYCONTROLLER_H

#include <QVariantMap>

class LzScene;

class LzSceneTopologyQueryController
{
public:
    QVariantMap buildPowerTopologyBindingSnapshot(LzScene* scene) const;
    QVariantMap buildTopologyBindingSnapshot(LzScene* scene) const;
    QVariantMap queryTopology(LzScene* scene, const QVariantMap& request) const;
};

#endif
