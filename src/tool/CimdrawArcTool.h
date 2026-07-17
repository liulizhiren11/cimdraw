#ifndef CIMDRAWARCTOOL_H
#define CIMDRAWARCTOOL_H

#include "CimdrawTool.h"

class CimdrawArcTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawArcTool(QObject* parent);
    virtual ~CimdrawArcTool() override;

    virtual CimdrawDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif
