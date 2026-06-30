#ifndef LZPOWERLOADBREAKTOOL_H
#define LZPOWERLOADBREAKTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerLoadBreakTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerLoadBreakTool(QObject* parent = nullptr);
    ~LzPowerLoadBreakTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERLOADBREAKTOOL_H
