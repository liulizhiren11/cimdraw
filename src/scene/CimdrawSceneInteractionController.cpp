#include "CimdrawSceneInteractionController.h"

#include <QMimeData>
#include <QUndoCommand>

#include "Command/CimdrawAddCommand.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "tool/CimdrawTool.h"

bool CimdrawSceneInteractionController::dispatchMousePress(CimdrawScene* scene,
                                                      CimdrawTool* tool,
                                                      QGraphicsSceneMouseEvent* event) const
{
    if (!tool)
        return false;
    tool->onMousePress(scene, event);
    return true;
}

bool CimdrawSceneInteractionController::dispatchMouseMove(CimdrawScene* scene,
                                                     CimdrawTool* tool,
                                                     QGraphicsSceneMouseEvent* event) const
{
    if (!tool)
        return false;
    tool->onMouseMove(scene, event);
    return true;
}

bool CimdrawSceneInteractionController::dispatchMouseRelease(CimdrawScene* scene,
                                                        CimdrawTool* tool,
                                                        QGraphicsSceneMouseEvent* event) const
{
    if (!tool)
        return false;
    tool->onMouseRelease(scene, event);
    return true;
}

bool CimdrawSceneInteractionController::dispatchMouseDoubleClick(CimdrawScene* scene,
                                                            CimdrawTool* tool,
                                                            QGraphicsSceneMouseEvent* event) const
{
    if (!tool)
        return false;
    tool->onMouseDoubleClick(scene, event);
    return true;
}

bool CimdrawSceneInteractionController::handleMenuAction(CimdrawScene* scene, QAction* action) const
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

bool CimdrawSceneInteractionController::handleDragEnter(CimdrawScene* scene, const QMimeData* mimeData) const
{
    if (!scene || !acceptsTextDrop(mimeData))
        return false;
    scene->update();
    return true;
}

bool CimdrawSceneInteractionController::handleDragMove(const QMimeData* mimeData) const
{
    return acceptsTextDrop(mimeData);
}

bool CimdrawSceneInteractionController::handleDrop(CimdrawScene* scene,
                                              const QMimeData* mimeData,
                                              const QPointF& scenePos) const
{
    if (!mimeData)
        return false;
    if (!mimeData->hasText())
        return false;
    return handleTextDrop(scene, mimeData->text(), scenePos);
}

bool CimdrawSceneInteractionController::acceptsTextDrop(const QMimeData* mimeData) const
{
    return mimeData && mimeData->hasText();
}

bool CimdrawSceneInteractionController::handleTextDrop(CimdrawScene* scene,
                                                  const QString& text,
                                                  const QPointF& scenePos) const
{
    if (!scene || text.isEmpty())
        return false;

    scene->clearSelection();

    CimdrawTool* tool = CimdrawToolManager::getInstance()->changeTool(text);
    if (!tool)
        return false;

    QRectF rc(scenePos, QSizeF(100, 100));
    QGraphicsItem* object = tool->createObject(scene, &rc);
    if (!object)
        return false;

    object->setPos(scenePos);

    const QList<QGraphicsItem*> previous = scene->getSelections();
    scene->addSelection(object);
    CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);

    CimdrawView* view = scene->getView();
    if (!view || !view->getStack())
        return false;

    view->setModified(true);
    view->getStack()->push(new CimdrawAddCommand(object, scene, previous));
    emit view->editChanged();
    scene->update();
    return true;
}
