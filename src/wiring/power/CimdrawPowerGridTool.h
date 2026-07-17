#ifndef CIMDRAWPOWERGRIDTOOL_H
#define CIMDRAWPOWERGRIDTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerGridTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerGridTool(QObject* parent = nullptr);
    ~CimdrawPowerGridTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERGRIDTOOL_H
