#ifndef LZPOWERLOADTOOL_H
#define LZPOWERLOADTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerLoadTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerLoadTool(QObject* parent = nullptr);
    ~LzPowerLoadTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERLOADTOOL_H
