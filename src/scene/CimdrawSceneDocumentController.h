#ifndef CIMDRAWSCENEDOCUMENTCONTROLLER_H
#define CIMDRAWSCENEDOCUMENTCONTROLLER_H

#include "CimdrawViewConfig.h"

class QDomDocument;
class QString;
class CimdrawScene;

class CimdrawSceneDocumentController
{
public:
    bool load(CimdrawScene* scene, const QString& filename, QString* inoutErrorMsg) const;
    bool toDomDocument(const QString& filename, QDomDocument* dom, QString* inoutErrorMsg) const;
    bool save(CimdrawScene* scene, const QString& filename) const;

    FILE_TYPE sceneType(FILE_TYPE currentType) const;
    void setSceneType(FILE_TYPE* currentType, FILE_TYPE newType) const;
};

#endif
