#ifndef CIMDRAWPOWERSVGCOMPTOOL_H
#define CIMDRAWPOWERSVGCOMPTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerSvgCompTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerSvgCompTool(QObject* parent = nullptr);
    ~CimdrawPowerSvgCompTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERSVGCOMPTOOL_H
