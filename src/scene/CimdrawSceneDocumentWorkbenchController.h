#ifndef CIMDRAWSCENEDOCUMENTWORKBENCHCONTROLLER_H
#define CIMDRAWSCENEDOCUMENTWORKBENCHCONTROLLER_H

#include "CimdrawViewConfig.h"

#include <QList>

class QDomDocument;
class QGraphicsItem;
class QGraphicsItemGroup;
class QString;
class CimdrawScene;

class CimdrawSceneDocumentWorkbenchController
{
public:
    bool load(CimdrawScene* scene, const QString& filename, QString* inoutErrorMsg) const;
    bool toDomDocument(const QString& filename, QDomDocument* dom, QString* inoutErrorMsg) const;
    bool save(CimdrawScene* scene, const QString& filename) const;

    FILE_TYPE sceneType(FILE_TYPE currentType) const;
    void setSceneType(FILE_TYPE* currentType, FILE_TYPE newType) const;

    QGraphicsItemGroup* createGroup(CimdrawScene* scene, const QList<QGraphicsItem*>& items) const;
};

#endif
