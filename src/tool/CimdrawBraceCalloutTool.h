#ifndef CIMDRAWBRACECALLOUTTOOL_H
#define CIMDRAWBRACECALLOUTTOOL_H

#include "CimdrawTool.h"

class CimdrawBraceCalloutTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawBraceCalloutTool(QObject* parent);
    ~CimdrawBraceCalloutTool() override;

    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif
