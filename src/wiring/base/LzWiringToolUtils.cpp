#include "LzWiringToolUtils.h"
#include "LzScene.h"
#include "LzView.h"
#include "Command/LzAddCommand.h"

#include <QGraphicsSceneMouseEvent>

void lzSldPushAdd(LzScene* scene, QGraphicsItem* pObj, QGraphicsSceneMouseEvent* evt)
{
    if (!pObj || !scene || !evt)
        return;
    pObj->setPos(evt->scenePos());
    LzView* view = scene->getView();
    if (!view)
        return;
    view->setModified(true);
    QList<QGraphicsItem*> list = scene->getSelections();
    scene->addSelection(pObj);
    QUndoCommand* command = new LzAddCommand(pObj, scene, list);
    view->getStack()->push(command);
    emit view->editChanged();
}
