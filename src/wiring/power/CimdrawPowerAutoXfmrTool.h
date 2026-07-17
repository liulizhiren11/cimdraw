#ifndef CIMDRAWPOWERAUTOXFMRTOOL_H
#define CIMDRAWPOWERAUTOXFMRTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerAutoXfmrTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerAutoXfmrTool(QObject* parent = nullptr);
    ~CimdrawPowerAutoXfmrTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERAUTOXFMRTOOL_H
