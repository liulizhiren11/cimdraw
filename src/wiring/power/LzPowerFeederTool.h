#ifndef LZPOWERPOWERFEEDERTOOL_H
#define LZPOWERPOWERFEEDERTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerFeederTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerFeederTool(QObject* parent = nullptr);
    ~LzPowerFeederTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERPOWERFEEDERTOOL_H
