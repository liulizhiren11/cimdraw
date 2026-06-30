#ifndef LZSCENEDOCUMENTWORKBENCHCONTROLLER_H
#define LZSCENEDOCUMENTWORKBENCHCONTROLLER_H

#include "LzViewConfig.h"

#include <QList>

class QDomDocument;
class QGraphicsItem;
class QGraphicsItemGroup;
class QString;
class LzScene;

class LzSceneDocumentWorkbenchController
{
public:
    bool load(LzScene* scene, const QString& filename, QString* inoutErrorMsg) const;
    bool toDomDocument(const QString& filename, QDomDocument* dom, QString* inoutErrorMsg) const;
    bool save(LzScene* scene, const QString& filename) const;

    FILE_TYPE sceneType(FILE_TYPE currentType) const;
    void setSceneType(FILE_TYPE* currentType, FILE_TYPE newType) const;

    QGraphicsItemGroup* createGroup(LzScene* scene, const QList<QGraphicsItem*>& items) const;
};

#endif
