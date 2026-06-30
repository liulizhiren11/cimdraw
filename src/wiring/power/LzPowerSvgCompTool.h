#ifndef LZPOWERSVGCOMPTOOL_H
#define LZPOWERSVGCOMPTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerSvgCompTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerSvgCompTool(QObject* parent = nullptr);
    ~LzPowerSvgCompTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERSVGCOMPTOOL_H
