#ifndef CIMDRAWTEXTTOOL_H
#define CIMDRAWTEXTTOOL_H

#include "CimdrawTool.h"

class CimdrawTextTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawTextTool(QObject* parent);
    virtual ~CimdrawTextTool() override;

    virtual CimdrawDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif
