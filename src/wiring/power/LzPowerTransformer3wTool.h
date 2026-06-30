#ifndef LZPOWERTRANSFORMER3WTOOL_H
#define LZPOWERTRANSFORMER3WTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerTransformer3wTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerTransformer3wTool(QObject* parent = nullptr);
    ~LzPowerTransformer3wTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERTRANSFORMER3WTOOL_H
