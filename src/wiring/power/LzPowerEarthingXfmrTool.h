#ifndef LZPOWEREARTHINGXFMRTOOL_H
#define LZPOWEREARTHINGXFMRTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerEarthingXfmrTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerEarthingXfmrTool(QObject* parent = nullptr);
    ~LzPowerEarthingXfmrTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWEREARTHINGXFMRTOOL_H
