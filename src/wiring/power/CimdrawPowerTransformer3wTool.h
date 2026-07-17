#ifndef CIMDRAWPOWERTRANSFORMER3WTOOL_H
#define CIMDRAWPOWERTRANSFORMER3WTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerTransformer3wTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerTransformer3wTool(QObject* parent = nullptr);
    ~CimdrawPowerTransformer3wTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERTRANSFORMER3WTOOL_H
