#ifndef CIMDRAWPOWERMOTORTOOL_H
#define CIMDRAWPOWERMOTORTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerMotorTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerMotorTool(QObject* parent = nullptr);
    ~CimdrawPowerMotorTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERMOTORTOOL_H
