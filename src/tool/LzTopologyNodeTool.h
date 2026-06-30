#ifndef LZTOPOLOGYNODETOOL_H
#define LZTOPOLOGYNODETOOL_H

#include "LzTool.h"

class LzTopologyNodeTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzTopologyNodeTool(QObject* parent);
    ~LzTopologyNodeTool() override;

    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif
