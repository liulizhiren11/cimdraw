#ifndef CIMDRAWPOWERSPLITREACTORTOOL_H
#define CIMDRAWPOWERSPLITREACTORTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerSplitReactorTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerSplitReactorTool(QObject* parent = nullptr);
    ~CimdrawPowerSplitReactorTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERSPLITREACTORTOOL_H
