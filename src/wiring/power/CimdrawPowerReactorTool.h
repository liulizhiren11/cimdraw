#ifndef CIMDRAWPOWERREACTORTOOL_H
#define CIMDRAWPOWERREACTORTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerReactorTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerReactorTool(QObject* parent = nullptr);
    ~CimdrawPowerReactorTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERREACTORTOOL_H
