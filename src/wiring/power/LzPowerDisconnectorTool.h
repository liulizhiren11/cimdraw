#ifndef LZPOWERDISCONNECTORTOOL_H
#define LZPOWERDISCONNECTORTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerDisconnectorTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerDisconnectorTool(QObject* parent = nullptr);
    ~LzPowerDisconnectorTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERDISCONNECTORTOOL_H
