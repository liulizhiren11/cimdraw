#ifndef LZPOWERFUSETOOL_H
#define LZPOWERFUSETOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerFuseTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerFuseTool(QObject* parent = nullptr);
    ~LzPowerFuseTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERFUSETOOL_H
