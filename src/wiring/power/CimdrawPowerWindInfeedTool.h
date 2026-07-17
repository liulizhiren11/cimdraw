#ifndef CIMDRAWPOWERWINDINFEEDTOOL_H
#define CIMDRAWPOWERWINDINFEEDTOOL_H

#include "Tool/CimdrawTool.h"

class CimdrawScene;

class CimdrawPowerWindInfeedTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawPowerWindInfeedTool(QObject* parent = nullptr);
    ~CimdrawPowerWindInfeedTool() override;
    CimdrawDrawTypeId getDrawType() const override;
    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    QIcon getIcon(const QSize& size, const QVariant& para) const override;
    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
};

#endif // CIMDRAWPOWERWINDINFEEDTOOL_H
