#ifndef CIMDRAWSCENETOPOLOGYQUERYCONTROLLER_H
#define CIMDRAWSCENETOPOLOGYQUERYCONTROLLER_H

#include <QVariantMap>

class CimdrawScene;

class CimdrawSceneTopologyQueryController
{
public:
    QVariantMap buildPowerTopologyBindingSnapshot(CimdrawScene* scene) const;
    QVariantMap buildTopologyBindingSnapshot(CimdrawScene* scene) const;
    QVariantMap queryTopology(CimdrawScene* scene, const QVariantMap& request) const;
};

#endif
