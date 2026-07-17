#ifndef CIMDRAWELLIPSETOOL_H
#define CIMDRAWELLIPSETOOL_H

#include "CimdrawTool.h"

class CimdrawEllipseTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawEllipseTool(QObject* parent);
    virtual ~CimdrawEllipseTool() override;

    virtual CimdrawDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};


#endif
