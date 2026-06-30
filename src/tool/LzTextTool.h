#ifndef LZTEXTTOOL_H
#define LZTEXTTOOL_H

#include "LzTool.h"

class LzTextTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzTextTool(QObject* parent);
    virtual ~LzTextTool() override;

    virtual LzDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif
