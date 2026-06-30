#ifndef LZPOWERPVINFEEDTOOL_H
#define LZPOWERPVINFEEDTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerPvInfeedTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerPvInfeedTool(QObject* parent = nullptr);
    ~LzPowerPvInfeedTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERPVINFEEDTOOL_H
