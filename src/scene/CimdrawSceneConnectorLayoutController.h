#ifndef CIMDRAWSCENECONNECTORLAYOUTCONTROLLER_H
#define CIMDRAWSCENECONNECTORLAYOUTCONTROLLER_H

#include <QList>

class CimdrawConnectLine;
class CimdrawScene;

class CimdrawSceneConnectorLayoutController
{
public:
    QList<CimdrawConnectLine*> expandNudgingScope(CimdrawScene* scene,
                                             const QList<CimdrawConnectLine*>& seedLines) const;
    bool applyConnectorNudging(CimdrawScene* scene,
                               const QList<CimdrawConnectLine*>& seedLines = {}) const;
};

#endif
