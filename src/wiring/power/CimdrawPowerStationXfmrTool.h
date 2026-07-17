#ifndef CIMDRAWPOWERSTATIONXFMRTOOL_H
#define CIMDRAWPOWERSTATIONXFMRTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerStationXfmrTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerStationXfmrTool(QObject* parent = nullptr);
    ~CimdrawPowerStationXfmrTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERSTATIONXFMRTOOL_H
