#ifndef LZSCENEDOCUMENTSLOTWORKBENCHCONTROLLER_H
#define LZSCENEDOCUMENTSLOTWORKBENCHCONTROLLER_H

#include "LzViewConfig.h"

#include <QList>

class QDomDocument;
class QGraphicsItem;
class QGraphicsItemGroup;
class QString;
class LzScene;

class LzSceneDocumentSlotWorkbenchController
{
public:
    enum class DocumentAction
    {
        Load,
        Save,
        ToDomDocument
    };

    enum class SceneTypeAction
    {
        Get,
        Set
    };

    enum class GroupAction
    {
        CreateGroup
    };

    bool dispatch(LzScene* scene,
                  DocumentAction action,
                  const QString& filename,
                  QString* inoutErrorMsg = nullptr) const;
    FILE_TYPE dispatchSceneType(LzScene* scene,
                                SceneTypeAction action,
                                FILE_TYPE type = FILE_TYPE::LZ_DRAW) const;
    QGraphicsItemGroup* dispatch(LzScene* scene,
                                 GroupAction action,
                                 const QList<QGraphicsItem*>& items) const;
    bool dispatch(DocumentAction action,
                  const QString& filename,
                  QDomDocument* dom,
                  QString* inoutErrorMsg = nullptr) const;

    bool load(LzScene* scene, const QString& filename, QString* inoutErrorMsg) const;
    bool toDomDocument(const QString& filename, QDomDocument* dom, QString* inoutErrorMsg) const;
    bool save(LzScene* scene, const QString& filename) const;

    FILE_TYPE sceneType(const LzScene* scene) const;
    bool setSceneType(LzScene* scene, FILE_TYPE type) const;

    QGraphicsItemGroup* createGroup(LzScene* scene, const QList<QGraphicsItem*>& items) const;
};

#endif
