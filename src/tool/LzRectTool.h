#ifndef LZRECTTOOL_H
#define LZRECTTOOL_H

#include "LzTool.h"

class LzRectTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzRectTool(QObject* parent);
    virtual ~LzRectTool() override;

    virtual LzDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif
