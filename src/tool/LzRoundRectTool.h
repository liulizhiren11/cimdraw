#ifndef LZROUNDRECTTOOL_H
#define LZROUNDRECTTOOL_H

#include "LzTool.h"

class LzRoundRectTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzRoundRectTool(QObject* parent);
    virtual ~LzRoundRectTool() override;

    virtual LzDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};


#endif
