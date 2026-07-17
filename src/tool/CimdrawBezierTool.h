#ifndef CIMDRAWBEZIERTOOL_H
#define CIMDRAWBEZIERTOOL_H

#include "CimdrawTool.h"

class CimdrawBezier;
class CimdrawBezierTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawBezierTool(QObject* parent);
    virtual ~CimdrawBezierTool() override;

    virtual CimdrawDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseMove(CimdrawScene* scene, QGraphicsSceneMouseEvent *evt) override;
private:
    CimdrawBezier* bezier;
    QPointF downPoint;
    QPointF lastPoint;
};

#endif


