#include "scene/LzSceneEditSlotWorkbenchController.h"

#include "LzScene.h"
#include "scene/LzSceneEditWorkbenchController.h"

namespace {

template <typename Fn>
bool runSceneEdit(LzScene* scene, Fn&& fn)
{
    if (!scene)
        return false;
    return fn(scene);
}

} // namespace

bool LzSceneEditSlotWorkbenchController::dispatch(LzScene* scene, Action action) const
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

bool LzSceneEditSlotWorkbenchController::dispatchShortcut(LzScene* scene,
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

bool LzSceneEditSlotWorkbenchController::alignLeft(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.alignLeft(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::alignRight(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.alignRight(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::alignTop(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.alignTop(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::alignBottom(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.alignBottom(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::alignVCenter(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.alignVCenter(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::alignHCenter(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.alignHCenter(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::autoCol(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.autoCol(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::autoRow(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.autoRow(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::sameWidth(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.sameWidth(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::sameHeight(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.sameHeight(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::sameSize(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.sameSize(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::shortcutLeft(LzScene* scene, const QPointF& delta) const
{
    return runSceneEdit(scene, [&delta](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.shortcutLeft(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene), delta);
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::shortcutRight(LzScene* scene, const QPointF& delta) const
{
    return runSceneEdit(scene, [&delta](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.shortcutRight(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene), delta);
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::shortcutUp(LzScene* scene, const QPointF& delta) const
{
    return runSceneEdit(scene, [&delta](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.shortcutUp(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene), delta);
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::shortcutDown(LzScene* scene, const QPointF& delta) const
{
    return runSceneEdit(scene, [&delta](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.shortcutDown(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene), delta);
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::cut(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.cut(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::copy(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.copy(currentScene->getSelections());
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::paste(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.paste(currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::remove(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.remove(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::levelUp(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.levelUp(currentScene->getSelections(), currentScene, currentScene);
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::levelDown(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.levelDown(currentScene->getSelections(), currentScene, currentScene);
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::toTop(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.toTop(currentScene->getSelections(), currentScene, currentScene);
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::toBottom(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.toBottom(currentScene->getSelections(), currentScene, currentScene);
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::rotate(LzScene* scene, qreal angleDegrees) const
{
    return runSceneEdit(scene, [angleDegrees](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.rotate(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene), angleDegrees);
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::group(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.group(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}

bool LzSceneEditSlotWorkbenchController::ungroup(LzScene* scene) const
{
    return runSceneEdit(scene, [](LzScene* currentScene)
    {
        LzSceneEditWorkbenchController controller;
        controller.ungroup(currentScene->getSelections(), currentScene, controller.stackFromScene(currentScene));
        return true;
    });
}
