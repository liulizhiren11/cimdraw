#ifndef LZPOWERAUTOXFMRTOOL_H
#define LZPOWERAUTOXFMRTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerAutoXfmrTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerAutoXfmrTool(QObject* parent = nullptr);
    ~LzPowerAutoXfmrTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERAUTOXFMRTOOL_H
