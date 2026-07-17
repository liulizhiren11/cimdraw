#ifndef CIMDRAWPOWERARCCOILTOOL_H
#define CIMDRAWPOWERARCCOILTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerArcCoilTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerArcCoilTool(QObject* parent = nullptr);
    ~CimdrawPowerArcCoilTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERARCCOILTOOL_H
