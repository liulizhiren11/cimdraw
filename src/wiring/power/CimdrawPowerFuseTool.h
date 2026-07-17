#ifndef CIMDRAWPOWERFUSETOOL_H
#define CIMDRAWPOWERFUSETOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerFuseTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerFuseTool(QObject* parent = nullptr);
    ~CimdrawPowerFuseTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERFUSETOOL_H
