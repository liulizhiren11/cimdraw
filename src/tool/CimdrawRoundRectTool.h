#ifndef CIMDRAWROUNDRECTTOOL_H
#define CIMDRAWROUNDRECTTOOL_H

#include "CimdrawTool.h"

class CimdrawRoundRectTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawRoundRectTool(QObject* parent);
    virtual ~CimdrawRoundRectTool() override;

    virtual CimdrawDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};


#endif
