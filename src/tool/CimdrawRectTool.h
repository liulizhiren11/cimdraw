#ifndef CIMDRAWRECTTOOL_H
#define CIMDRAWRECTTOOL_H

#include "CimdrawTool.h"

class CimdrawRectTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawRectTool(QObject* parent);
    virtual ~CimdrawRectTool() override;

    virtual CimdrawDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif
