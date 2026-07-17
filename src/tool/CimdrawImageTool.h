#ifndef CIMDRAWIMAGETOOL_H
#define CIMDRAWIMAGETOOL_H

#include "CimdrawTool.h"

class CimdrawScene;

class CimdrawImageTool : public CimdrawTool
{
public:
    explicit CimdrawImageTool(QObject* parent);
    virtual ~CimdrawImageTool() override;

    virtual CimdrawDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;

    void setFilePath(const QString& path);
    QString filePath() const;
protected:
    QString imagePath;
};


#endif
