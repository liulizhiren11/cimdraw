#ifndef CIMDRAWPOLYLINETOOL_H
#define CIMDRAWPOLYLINETOOL_H

#include "CimdrawTool.h"

class CimdrawPolyline;
class CimdrawPolylineTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPolylineTool(QObject* parent);
    virtual ~CimdrawPolylineTool() override;

    virtual CimdrawDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseMove(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
private:
    CimdrawPolyline* polyline;
    QPointF downPoint;
    QPointF lastPoint;
};

#endif
