#ifndef CIMDRAWPOWERPVINFEEDTOOL_H
#define CIMDRAWPOWERPVINFEEDTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerPvInfeedTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerPvInfeedTool(QObject* parent = nullptr);
    ~CimdrawPowerPvInfeedTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERPVINFEEDTOOL_H
