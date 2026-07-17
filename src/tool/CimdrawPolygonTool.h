#ifndef CIMDRAWPOLYGONTOOL_H
#define CIMDRAWPOLYGONTOOL_H

#include "CimdrawTool.h"

class CimdrawPolygon;
class CimdrawPolygonTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPolygonTool(QObject* parent);
    virtual ~CimdrawPolygonTool() override;

    virtual CimdrawDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseMove(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
private:
    CimdrawPolygon* polygon;
    QPointF downPoint;
    QPointF lastPoint;
};
#endif
