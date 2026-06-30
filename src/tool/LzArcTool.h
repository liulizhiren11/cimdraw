#ifndef LZARCTOOL_H
#define LZARCTOOL_H

#include "LzTool.h"

class LzArcTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzArcTool(QObject* parent);
    virtual ~LzArcTool() override;

    virtual LzDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif
