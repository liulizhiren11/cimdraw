#ifndef LZPOWERJUNCTIONTOOL_H
#define LZPOWERJUNCTIONTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerJunctionTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerJunctionTool(QObject* parent = nullptr);
    ~LzPowerJunctionTool() override;

    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERJUNCTIONTOOL_H
