#ifndef CIMDRAWPOWERLOADTOOL_H
#define CIMDRAWPOWERLOADTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerLoadTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerLoadTool(QObject* parent = nullptr);
    ~CimdrawPowerLoadTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERLOADTOOL_H
