#include "CimdrawWiringToolUtils.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "Command/CimdrawAddCommand.h"

#include <QGraphicsSceneMouseEvent>

void cimdrawSldPushAdd(CimdrawScene* scene, QGraphicsItem* pObj, QGraphicsSceneMouseEvent* evt)
{
    if (!pObj || !scene || !evt)
        return;
    pObj->setPos(evt->scenePos());
    CimdrawView* view = scene->getView();
    if (!view)
        return;
    view->setModified(true);
    QList<QGraphicsItem*> list = scene->getSelections();
    scene->addSelection(pObj);
    QUndoCommand* command = new CimdrawAddCommand(pObj, scene, list);
    view->getStack()->push(command);
    emit view->editChanged();
}
