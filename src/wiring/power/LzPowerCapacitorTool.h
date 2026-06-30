#ifndef LZPOWERCAPACITORTOOL_H
#define LZPOWERCAPACITORTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerCapacitorTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerCapacitorTool(QObject* parent = nullptr);
    ~LzPowerCapacitorTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERCAPACITORTOOL_H
