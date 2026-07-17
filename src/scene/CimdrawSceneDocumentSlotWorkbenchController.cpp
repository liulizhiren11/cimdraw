#include "scene/CimdrawSceneDocumentSlotWorkbenchController.h"

#include "CimdrawScene.h"
#include "scene/CimdrawSceneDocumentWorkbenchController.h"

namespace {

template <typename Fn, typename Fallback>
auto runSceneDocument(CimdrawScene* scene, Fn&& fn, Fallback&& fallback) -> decltype(fn(scene))
{
    if (!scene)
        return fallback();
    return fn(scene);
}

template <typename Fn, typename Fallback>
auto runConstSceneDocument(const CimdrawScene* scene, Fn&& fn, Fallback&& fallback) -> decltype(fn(scene))
{
    if (!scene)
        return fallback();
    return fn(scene);
}

} // namespace

bool CimdrawSceneDocumentSlotWorkbenchController::dispatch(CimdrawScene* scene,
                                                      DocumentAction action,
                                                      const QString& filename,
                                                      QString* inoutErrorMsg) const
{
    switch (action)
    {
    case DocumentAction::Load:
        return load(scene, filename, inoutErrorMsg);
    case DocumentAction::Save:
        return save(scene, filename);
    case DocumentAction::ToDomDocument:
        break;
    }

    return false;
}

QGraphicsItemGroup* CimdrawSceneDocumentSlotWorkbenchController::dispatch(
    CimdrawScene* scene,
    GroupAction action,
    const QList<QGraphicsItem*>& items) const
{
    switch (action)
    {
    case GroupAction::CreateGroup:
        return createGroup(scene, items);
    }

    return nullptr;
}

bool CimdrawSceneDocumentSlotWorkbenchController::dispatch(DocumentAction action,
                                                      const QString& filename,
                                                      QDomDocument* dom,
                                                      QString* inoutErrorMsg) const
{
    switch (action)
    {
    case DocumentAction::ToDomDocument:
        return toDomDocument(filename, dom, inoutErrorMsg);
    case DocumentAction::Load:
    case DocumentAction::Save:
        break;
    }

    return false;
}

FILE_TYPE CimdrawSceneDocumentSlotWorkbenchController::dispatchSceneType(CimdrawScene* scene,
                                                                    SceneTypeAction action,
                                                                    FILE_TYPE type) const
{
    switch (action)
    {
    case SceneTypeAction::Get:
        return sceneType(scene);
    case SceneTypeAction::Set:
        setSceneType(scene, type);
        return sceneType(scene);
    }

    return FILE_TYPE::CIMDRAW_DRAW;
}

bool CimdrawSceneDocumentSlotWorkbenchController::load(CimdrawScene* scene,
                                                  const QString& filename,
                                                  QString* inoutErrorMsg) const
{
    return runSceneDocument(scene, [&](CimdrawScene* currentScene)
    {
        CimdrawSceneDocumentWorkbenchController controller;
        return controller.load(currentScene, filename, inoutErrorMsg);
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneDocumentSlotWorkbenchController::toDomDocument(const QString& filename,
                                                           QDomDocument* dom,
                                                           QString* inoutErrorMsg) const
{
    CimdrawSceneDocumentWorkbenchController controller;
    return controller.toDomDocument(filename, dom, inoutErrorMsg);
}

bool CimdrawSceneDocumentSlotWorkbenchController::save(CimdrawScene* scene, const QString& filename) const
{
    return runSceneDocument(scene, [&](CimdrawScene* currentScene)
    {
        CimdrawSceneDocumentWorkbenchController controller;
        return controller.save(currentScene, filename);
    }, []()
    {
        return false;
    });
}

FILE_TYPE CimdrawSceneDocumentSlotWorkbenchController::sceneType(const CimdrawScene* scene) const
{
    return runConstSceneDocument(scene, [](const CimdrawScene* currentScene)
    {
        CimdrawSceneDocumentWorkbenchController controller;
        return controller.sceneType(currentScene->sceneTypeStorage());
    }, []()
    {
        return FILE_TYPE::CIMDRAW_DRAW;
    });
}

bool CimdrawSceneDocumentSlotWorkbenchController::setSceneType(CimdrawScene* scene, FILE_TYPE type) const
{
    return runSceneDocument(scene, [type](CimdrawScene* currentScene)
    {
        CimdrawSceneDocumentWorkbenchController controller;
        FILE_TYPE& currentType = currentScene->sceneTypeStorage();
        controller.setSceneType(&currentType, type);
        return true;
    }, []()
    {
        return false;
    });
}

QGraphicsItemGroup* CimdrawSceneDocumentSlotWorkbenchController::createGroup(CimdrawScene* scene,
                                                                        const QList<QGraphicsItem*>& items) const
{
    return runSceneDocument(scene, [&](CimdrawScene* currentScene)
    {
        CimdrawSceneDocumentWorkbenchController controller;
        return controller.createGroup(currentScene, items);
    }, []() -> QGraphicsItemGroup*
    {
        return nullptr;
    });
}
