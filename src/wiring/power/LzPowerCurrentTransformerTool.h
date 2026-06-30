#ifndef LZPOWERCURRENTTRANSFORMERTOOL_H
#define LZPOWERCURRENTTRANSFORMERTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerCurrentTransformerTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerCurrentTransformerTool(QObject* parent = nullptr);
    ~LzPowerCurrentTransformerTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERCURRENTTRANSFORMERTOOL_H
