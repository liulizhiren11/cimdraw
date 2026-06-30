#ifndef LZPOWERGRIDTOOL_H
#define LZPOWERGRIDTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerGridTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerGridTool(QObject* parent = nullptr);
    ~LzPowerGridTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERGRIDTOOL_H
