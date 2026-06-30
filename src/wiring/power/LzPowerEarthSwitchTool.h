#ifndef LZPOWEREARTHSWITCHTOOL_H
#define LZPOWEREARTHSWITCHTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerEarthSwitchTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerEarthSwitchTool(QObject* parent = nullptr);
    ~LzPowerEarthSwitchTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWEREARTHSWITCHTOOL_H
