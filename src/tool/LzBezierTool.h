#ifndef LZBEZIERTOOL_H
#define LZBEZIERTOOL_H

#include "LzTool.h"

class LzBezier;
class LzBezierTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzBezierTool(QObject* parent);
    virtual ~LzBezierTool() override;

    virtual LzDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent *evt) override;
private:
    LzBezier* bezier;
    QPointF downPoint;
    QPointF lastPoint;
};

#endif


