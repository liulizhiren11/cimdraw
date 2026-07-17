#ifndef CIMDRAWTOPOLOGYNODETOOL_H
#define CIMDRAWTOPOLOGYNODETOOL_H

#include "CimdrawTool.h"

class CimdrawTopologyNodeTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawTopologyNodeTool(QObject* parent);
    ~CimdrawTopologyNodeTool() override;

    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif
