#ifndef LZPOWERESSTOOL_H
#define LZPOWERESSTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerEssTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerEssTool(QObject* parent = nullptr);
    ~LzPowerEssTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERESSTOOL_H
