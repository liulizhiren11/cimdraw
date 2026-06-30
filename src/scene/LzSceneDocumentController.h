#ifndef LZSCENEDOCUMENTCONTROLLER_H
#define LZSCENEDOCUMENTCONTROLLER_H

#include "LzViewConfig.h"

class QDomDocument;
class QString;
class LzScene;

class LzSceneDocumentController
{
public:
    bool load(LzScene* scene, const QString& filename, QString* inoutErrorMsg) const;
    bool toDomDocument(const QString& filename, QDomDocument* dom, QString* inoutErrorMsg) const;
    bool save(LzScene* scene, const QString& filename) const;

    FILE_TYPE sceneType(FILE_TYPE currentType) const;
    void setSceneType(FILE_TYPE* currentType, FILE_TYPE newType) const;
};

#endif
