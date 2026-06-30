#ifndef LZPOWERGROUNDTOOL_H
#define LZPOWERGROUNDTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerGroundTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerGroundTool(QObject* parent = nullptr);
    ~LzPowerGroundTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERGROUNDTOOL_H
