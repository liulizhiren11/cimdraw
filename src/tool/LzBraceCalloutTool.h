#ifndef LZBRACECALLOUTTOOL_H
#define LZBRACECALLOUTTOOL_H

#include "LzTool.h"

class LzBraceCalloutTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzBraceCalloutTool(QObject* parent);
    ~LzBraceCalloutTool() override;

    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif
