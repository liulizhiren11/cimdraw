#include "scene/LzSceneDocumentSlotWorkbenchController.h"

#include "LzScene.h"
#include "scene/LzSceneDocumentWorkbenchController.h"

namespace {

template <typename Fn, typename Fallback>
auto runSceneDocument(LzScene* scene, Fn&& fn, Fallback&& fallback) -> decltype(fn(scene))
{
    if (!scene)
        return fallback();
    return fn(scene);
}

template <typename Fn, typename Fallback>
auto runConstSceneDocument(const LzScene* scene, Fn&& fn, Fallback&& fallback) -> decltype(fn(scene))
{
    if (!scene)
        return fallback();
    return fn(scene);
}

} // namespace

bool LzSceneDocumentSlotWorkbenchController::dispatch(LzScene* scene,
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

QGraphicsItemGroup* LzSceneDocumentSlotWorkbenchController::dispatch(
    LzScene* scene,
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

bool LzSceneDocumentSlotWorkbenchController::dispatch(DocumentAction action,
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

FILE_TYPE LzSceneDocumentSlotWorkbenchController::dispatchSceneType(LzScene* scene,
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

    return FILE_TYPE::LZ_DRAW;
}

bool LzSceneDocumentSlotWorkbenchController::load(LzScene* scene,
                                                  const QString& filename,
                                                  QString* inoutErrorMsg) const
{
    return runSceneDocument(scene, [&](LzScene* currentScene)
    {
        LzSceneDocumentWorkbenchController controller;
        return controller.load(currentScene, filename, inoutErrorMsg);
    }, []()
    {
        return false;
    });
}

bool LzSceneDocumentSlotWorkbenchController::toDomDocument(const QString& filename,
                                                           QDomDocument* dom,
                                                           QString* inoutErrorMsg) const
{
    LzSceneDocumentWorkbenchController controller;
    return controller.toDomDocument(filename, dom, inoutErrorMsg);
}

bool LzSceneDocumentSlotWorkbenchController::save(LzScene* scene, const QString& filename) const
{
    return runSceneDocument(scene, [&](LzScene* currentScene)
    {
        LzSceneDocumentWorkbenchController controller;
        return controller.save(currentScene, filename);
    }, []()
    {
        return false;
    });
}

FILE_TYPE LzSceneDocumentSlotWorkbenchController::sceneType(const LzScene* scene) const
{
    return runConstSceneDocument(scene, [](const LzScene* currentScene)
    {
        LzSceneDocumentWorkbenchController controller;
        return controller.sceneType(currentScene->sceneTypeStorage());
    }, []()
    {
        return FILE_TYPE::LZ_DRAW;
    });
}

bool LzSceneDocumentSlotWorkbenchController::setSceneType(LzScene* scene, FILE_TYPE type) const
{
    return runSceneDocument(scene, [type](LzScene* currentScene)
    {
        LzSceneDocumentWorkbenchController controller;
        FILE_TYPE& currentType = currentScene->sceneTypeStorage();
        controller.setSceneType(&currentType, type);
        return true;
    }, []()
    {
        return false;
    });
}

QGraphicsItemGroup* LzSceneDocumentSlotWorkbenchController::createGroup(LzScene* scene,
                                                                        const QList<QGraphicsItem*>& items) const
{
    return runSceneDocument(scene, [&](LzScene* currentScene)
    {
        LzSceneDocumentWorkbenchController controller;
        return controller.createGroup(currentScene, items);
    }, []() -> QGraphicsItemGroup*
    {
        return nullptr;
    });
}
