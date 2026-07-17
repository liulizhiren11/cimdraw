#ifndef CIMDRAWPOWERCURRENTTRANSFORMERTOOL_H
#define CIMDRAWPOWERCURRENTTRANSFORMERTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerCurrentTransformerTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerCurrentTransformerTool(QObject* parent = nullptr);
    ~CimdrawPowerCurrentTransformerTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERCURRENTTRANSFORMERTOOL_H
