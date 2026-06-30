#ifndef LZSCENECONNECTORLAYOUTCONTROLLER_H
#define LZSCENECONNECTORLAYOUTCONTROLLER_H

#include <QList>

class LzConnectLine;
class LzScene;

class LzSceneConnectorLayoutController
{
public:
    QList<LzConnectLine*> expandNudgingScope(LzScene* scene,
                                             const QList<LzConnectLine*>& seedLines) const;
    bool applyConnectorNudging(LzScene* scene,
                               const QList<LzConnectLine*>& seedLines = {}) const;
};

#endif
