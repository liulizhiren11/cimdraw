#ifndef CIMDRAWLINETOOL_H
#define CIMDRAWLINETOOL_H

#include "CimdrawTool.h"

class CimdrawLineToolPrivate;
class CimdrawLineTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawLineTool(QObject* parent);
    virtual ~CimdrawLineTool() override;
    
    virtual CimdrawDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseMove(CimdrawScene* scene, QGraphicsSceneMouseEvent *evt) override;
private:
    CimdrawLineToolPrivate* d_ptr;
};

#endif
