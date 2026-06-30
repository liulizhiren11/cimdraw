#ifndef LZPOLYLINETOOL_H
#define LZPOLYLINETOOL_H

#include "LzTool.h"

class LzPolyline;
class LzPolylineTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPolylineTool(QObject* parent);
    virtual ~LzPolylineTool() override;

    virtual LzDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
private:
    LzPolyline* polyline;
    QPointF downPoint;
    QPointF lastPoint;
};

#endif
