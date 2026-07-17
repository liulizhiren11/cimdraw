#ifndef CIMDRAWPOWERDISCONNECTORTOOL_H
#define CIMDRAWPOWERDISCONNECTORTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerDisconnectorTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerDisconnectorTool(QObject* parent = nullptr);
    ~CimdrawPowerDisconnectorTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERDISCONNECTORTOOL_H
