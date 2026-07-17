#ifndef CIMDRAWPOWERCAPACITORTOOL_H
#define CIMDRAWPOWERCAPACITORTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerCapacitorTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerCapacitorTool(QObject* parent = nullptr);
    ~CimdrawPowerCapacitorTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERCAPACITORTOOL_H
