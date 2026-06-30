#ifndef LZPOWERWINDINFEEDTOOL_H
#define LZPOWERWINDINFEEDTOOL_H

#include "Tool/LzTool.h"

class LzScene;

class LzPowerWindInfeedTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzPowerWindInfeedTool(QObject* parent = nullptr);
    ~LzPowerWindInfeedTool() override;
    LzDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // LZPOWERWINDINFEEDTOOL_H
