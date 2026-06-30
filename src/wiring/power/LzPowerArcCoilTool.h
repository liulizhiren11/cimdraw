#ifndef LZPOWERARCCOILTOOL_H
#define LZPOWERARCCOILTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerArcCoilTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerArcCoilTool(QObject* parent = nullptr);
    ~LzPowerArcCoilTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERARCCOILTOOL_H
