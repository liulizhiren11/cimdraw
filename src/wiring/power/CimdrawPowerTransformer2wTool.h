#ifndef CIMDRAWPOWERTRANSFORMER2WTOOL_H
#define CIMDRAWPOWERTRANSFORMER2WTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerTransformer2wTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerTransformer2wTool(QObject* parent = nullptr);
    ~CimdrawPowerTransformer2wTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERTRANSFORMER2WTOOL_H
