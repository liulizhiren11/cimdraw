#ifndef LZPOWERSTATIONXFMRTOOL_H
#define LZPOWERSTATIONXFMRTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerStationXfmrTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerStationXfmrTool(QObject* parent = nullptr);
    ~LzPowerStationXfmrTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERSTATIONXFMRTOOL_H
