#ifndef LZPOWERCIRCUITBREAKERTOOL_H
#define LZPOWERCIRCUITBREAKERTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerCircuitBreakerTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerCircuitBreakerTool(QObject* parent = nullptr);
    ~LzPowerCircuitBreakerTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERCIRCUITBREAKERTOOL_H
