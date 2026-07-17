#ifndef CIMDRAWPOWERLOADBREAKTOOL_H
#define CIMDRAWPOWERLOADBREAKTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerLoadBreakTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerLoadBreakTool(QObject* parent = nullptr);
    ~CimdrawPowerLoadBreakTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERLOADBREAKTOOL_H
