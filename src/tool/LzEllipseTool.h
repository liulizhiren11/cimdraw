#ifndef LZELLIPSETOOL_H
#define LZELLIPSETOOL_H

#include "LzTool.h"

class LzEllipseTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzEllipseTool(QObject* parent);
    virtual ~LzEllipseTool() override;

    virtual LzDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};


#endif
