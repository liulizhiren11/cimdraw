#ifndef CIMDRAWPOWERCABLETOOL_H
#define CIMDRAWPOWERCABLETOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerCableTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerCableTool(QObject* parent = nullptr);
    ~CimdrawPowerCableTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERCABLETOOL_H
