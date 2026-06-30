#ifndef LZIMAGETOOL_H
#define LZIMAGETOOL_H

#include "LzTool.h"

class LzScene;

class LzImageTool : public LzTool
{
public:
    explicit LzImageTool(QObject* parent);
    virtual ~LzImageTool() override;

    virtual LzDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;

    void setFilePath(const QString& path);
    QString filePath() const;
protected:
    QString imagePath;
};


#endif
