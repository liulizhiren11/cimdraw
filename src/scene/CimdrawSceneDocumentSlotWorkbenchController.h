#ifndef CIMDRAWSCENEDOCUMENTSLOTWORKBENCHCONTROLLER_H
#define CIMDRAWSCENEDOCUMENTSLOTWORKBENCHCONTROLLER_H

#include "CimdrawViewConfig.h"

#include <QList>

class QDomDocument;
class QGraphicsItem;
class QGraphicsItemGroup;
class QString;
class CimdrawScene;

class CimdrawSceneDocumentSlotWorkbenchController
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

    bool dispatch(CimdrawScene* scene,
                  DocumentAction action,
                  const QString& filename,
                  QString* inoutErrorMsg = nullptr) const;
    FILE_TYPE dispatchSceneType(CimdrawScene* scene,
                                SceneTypeAction action,
                                FILE_TYPE type = FILE_TYPE::CIMDRAW_DRAW) const;
    QGraphicsItemGroup* dispatch(CimdrawScene* scene,
                                 GroupAction action,
                                 const QList<QGraphicsItem*>& items) const;
    bool dispatch(DocumentAction action,
                  const QString& filename,
                  QDomDocument* dom,
                  QString* inoutErrorMsg = nullptr) const;

    bool load(CimdrawScene* scene, const QString& filename, QString* inoutErrorMsg) const;
    bool toDomDocument(const QString& filename, QDomDocument* dom, QString* inoutErrorMsg) const;
    bool save(CimdrawScene* scene, const QString& filename) const;

    FILE_TYPE sceneType(const CimdrawScene* scene) const;
    bool setSceneType(CimdrawScene* scene, FILE_TYPE type) const;

    QGraphicsItemGroup* createGroup(CimdrawScene* scene, const QList<QGraphicsItem*>& items) const;
};

#endif
