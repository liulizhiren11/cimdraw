#ifndef LZPOWERSPLITREACTORTOOL_H
#define LZPOWERSPLITREACTORTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerSplitReactorTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerSplitReactorTool(QObject* parent = nullptr);
    ~LzPowerSplitReactorTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERSPLITREACTORTOOL_H
