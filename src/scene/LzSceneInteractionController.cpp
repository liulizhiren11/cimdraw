#include "LzSceneInteractionController.h"

#include <QMimeData>
#include <QUndoCommand>

#include "Command/LzAddCommand.h"
#include "LzScene.h"
#include "LzView.h"
#include "tool/LzTool.h"

bool LzSceneInteractionController::dispatchMousePress(LzScene* scene,
                                                      LzTool* tool,
                                                      QGraphicsSceneMouseEvent* event) const
{
    if (!tool)
        return false;
    tool->onMousePress(scene, event);
    return true;
}

bool LzSceneInteractionController::dispatchMouseMove(LzScene* scene,
                                                     LzTool* tool,
                                                     QGraphicsSceneMouseEvent* event) const
{
    if (!tool)
        return false;
    tool->onMouseMove(scene, event);
    return true;
}

bool LzSceneInteractionController::dispatchMouseRelease(LzScene* scene,
                                                        LzTool* tool,
                                                        QGraphicsSceneMouseEvent* event) const
{
    if (!tool)
        return false;
    tool->onMouseRelease(scene, event);
    return true;
}

bool LzSceneInteractionController::dispatchMouseDoubleClick(LzScene* scene,
                                                            LzTool* tool,
                                                            QGraphicsSceneMouseEvent* event) const
{
    if (!tool)
        return false;
    tool->onMouseDoubleClick(scene, event);
    return true;
}

bool LzSceneInteractionController::handleMenuAction(LzScene* scene, QAction* action) const
{
    if (!scene || !action)
        return false;

    const QString text = action->text().trimmed();
    if (text == QStringLiteral("剪切"))
    {
        scene->editCut();
        return true;
    }
    if (text == QStringLiteral("复制"))
    {
        scene->editCopy();
        return true;
    }
    if (text == QStringLiteral("粘贴"))
    {
        scene->editPaste();
        return true;
    }
    if (text == QStringLiteral("删除"))
    {
        scene->editDelete();
        return true;
    }
    if (text == QStringLiteral("组合"))
    {
        scene->editGroup();
        return true;
    }
    if (text == QStringLiteral("取消组合"))
    {
        scene->editUnGroup();
        return true;
    }
    return false;
}

bool LzSceneInteractionController::handleDragEnter(LzScene* scene, const QMimeData* mimeData) const
{
    if (!scene || !acceptsTextDrop(mimeData))
        return false;
    scene->update();
    return true;
}

bool LzSceneInteractionController::handleDragMove(const QMimeData* mimeData) const
{
    return acceptsTextDrop(mimeData);
}

bool LzSceneInteractionController::handleDrop(LzScene* scene,
                                              const QMimeData* mimeData,
                                              const QPointF& scenePos) const
{
    if (!mimeData)
        return false;
    if (!mimeData->hasText())
        return false;
    return handleTextDrop(scene, mimeData->text(), scenePos);
}

bool LzSceneInteractionController::acceptsTextDrop(const QMimeData* mimeData) const
{
    return mimeData && mimeData->hasText();
}

bool LzSceneInteractionController::handleTextDrop(LzScene* scene,
                                                  const QString& text,
                                                  const QPointF& scenePos) const
{
    if (!scene || text.isEmpty())
        return false;

    scene->clearSelection();

    LzTool* tool = LzToolManager::getInstance()->changeTool(text);
    if (!tool)
        return false;

    QRectF rc(scenePos, QSizeF(100, 100));
    QGraphicsItem* object = tool->createObject(scene, &rc);
    if (!object)
        return false;

    object->setPos(scenePos);

    const QList<QGraphicsItem*> previous = scene->getSelections();
    scene->addSelection(object);
    LzToolManager::getInstance()->changeTool(LZ_SELECTION);

    LzView* view = scene->getView();
    if (!view || !view->getStack())
        return false;

    view->setModified(true);
    view->getStack()->push(new LzAddCommand(object, scene, previous));
    emit view->editChanged();
    scene->update();
    return true;
}
