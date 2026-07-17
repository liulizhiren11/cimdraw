#include "scene/CimdrawSceneEditSlotWorkbenchController.h"

#include "CimdrawScene.h"
#include "scene/CimdrawSceneEditWorkbenchController.h"

namespace {

template <typename Fn>
bool runSceneEdit(CimdrawScene* scene, Fn&& fn)
{
    if (!scene)
        return false;
    return fn(scene);
}

} // namespace

bool CimdrawSceneEditSlotWorkbenchController::dispatch(CimdrawScene* scene, Action action) const
{
    switch (action)
    {
    case Action::AlignLeft:
        return alignLeft(scene);
    case Action::AlignRight:
        return alignRight(scene);
    case Action::AlignTop:
        return alignTop(scene);
    case Action::AlignBottom:
        return alignBottom(scene);
    case Action::AlignVCenter:
        return alignVCenter(scene);
    case Action::AlignHCenter:
        return alignHCenter(scene);
    case Action::AutoCol:
        return autoCol(scene);
    case Action::AutoRow:
        return autoRow(scene);
    case Action::SameWidth:
        return sameWidth(scene);
    case Action::SameHeight:
        return sameHeight(scene);
    case Action::SameSize:
        return sameSize(scene);
    case Action::Cut:
        return cut(scene);
    case Action::Copy:
        return copy(scene);
    case Action::Paste:
        return paste(scene);
    case Action::Remove:
        return remove(scene);
    case Action::LevelUp:
        return levelUp(scene);
    case Action::LevelDown:
        return levelDown(scene);
    case Action::ToTop:
        return toTop(scene);
    case Action::ToBottom:
        return toBottom(scene);
    case Action::Group:
        return group(scene);
    case Action::Ungroup:
        return ungroup(scene);
    }

    return false;
}

bool CimdrawSceneEditSlotWorkbenchController::dispatchShortcut(CimdrawScene* scene,
                                                          ShortcutDirection direction,
                                                          const QPointF& delta) const
{
    switch (direction)
    {
    case ShortcutDirection::Left:
        return shortcutLeft(scene, delta);
    case ShortcutDirection::Right:
        return shortcutRight(scene, delta);
    case ShortcutDirection::Up:
        return shortcutUp(scene, delta);
    case ShortcutDirection::Down:
        return shortcutDown(scene, delta);
    }

    return false;
}

bool CimdrawSceneEditSlotWorkbenchController::alignLeft(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.alignLeft(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::alignRight(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.alignRight(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::alignTop(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.alignTop(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::alignBottom(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.alignBottom(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::alignVCenter(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.alignVCenter(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::alignHCenter(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.alignHCenter(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::autoCol(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.autoCol(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::autoRow(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.autoRow(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::sameWidth(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.sameWidth(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::sameHeight(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.sameHeight(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::sameSize(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.sameSize(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::shortcutLeft(CimdrawScene* scene, const QPointF& delta) const
{
    return runSceneEdit(scene, [&delta](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.shortcutLeft(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene), delta);
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::shortcutRight(CimdrawScene* scene, const QPointF& delta) const
{
    return runSceneEdit(scene, [&delta](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.shortcutRight(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene), delta);
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::shortcutUp(CimdrawScene* scene, const QPointF& delta) const
{
    return runSceneEdit(scene, [&delta](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.shortcutUp(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene), delta);
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::shortcutDown(CimdrawScene* scene, const QPointF& delta) const
{
    return runSceneEdit(scene, [&delta](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.shortcutDown(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene), delta);
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::cut(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.cut(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::copy(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.copy(currentScene->getSelections());
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::paste(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.paste(currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::remove(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.remove(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::levelUp(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.levelUp(currentScene->getSelections(), currentScene, currentScene);
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::levelDown(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.levelDown(currentScene->getSelections(), currentScene, currentScene);
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::toTop(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.toTop(currentScene->getSelections(), currentScene, currentScene);
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::toBottom(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.toBottom(currentScene->getSelections(), currentScene, currentScene);
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::rotate(CimdrawScene* scene, qreal angleDegrees) const
{
    return runSceneEdit(scene, [angleDegrees](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.rotate(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene), angleDegrees);
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::group(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.group(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool CimdrawSceneEditSlotWorkbenchController::ungroup(CimdrawScene* scene) const
{
    return runSceneEdit(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneEditWorkbenchController controller;
        controller.ungroup(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}
