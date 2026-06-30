#ifndef LZPOLYGONTOOL_H
#define LZPOLYGONTOOL_H

#include "LzTool.h"

class LzPolygon;
class LzPolygonTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPolygonTool(QObject* parent);
    virtual ~LzPolygonTool() override;

    virtual LzDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
private:
    LzPolygon* polygon;
    QPointF downPoint;
    QPointF lastPoint;
};
#endif
