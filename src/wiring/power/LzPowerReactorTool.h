#ifndef LZPOWERREACTORTOOL_H
#define LZPOWERREACTORTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerReactorTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerReactorTool(QObject* parent = nullptr);
    ~LzPowerReactorTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERREACTORTOOL_H
