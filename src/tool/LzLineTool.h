#ifndef LZLINETOOL_H
#define LZLINETOOL_H

#include "LzTool.h"

class LzLineToolPrivate;
class LzLineTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzLineTool(QObject* parent);
    virtual ~LzLineTool() override;
    
    virtual LzDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent *evt) override;
private:
    LzLineToolPrivate* d_ptr;
};

#endif
