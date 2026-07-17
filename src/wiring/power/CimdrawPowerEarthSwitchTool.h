#ifndef CIMDRAWPOWEREARTHSWITCHTOOL_H
#define CIMDRAWPOWEREARTHSWITCHTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerEarthSwitchTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerEarthSwitchTool(QObject* parent = nullptr);
    ~CimdrawPowerEarthSwitchTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWEREARTHSWITCHTOOL_H
