#ifndef CIMDRAWPOWERESSTOOL_H
#define CIMDRAWPOWERESSTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerEssTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerEssTool(QObject* parent = nullptr);
    ~CimdrawPowerEssTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERESSTOOL_H
