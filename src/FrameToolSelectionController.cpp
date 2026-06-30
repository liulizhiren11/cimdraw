#include "FrameToolSelectionController.h"

#include <QDebug>
#include <QListWidgetItem>

#include "LzListWidgetItem.h"
#include "LzScene.h"
#include "Tool/LzTool.h"

void FrameToolSelectionController::activateDrawTool(QListWidgetItem* item, LzScene* scene) const
{
    qDebug() << "onDrawToolChanged";
    if (!item || !scene || scene->getPaintState())
        return;

    auto* drawItem = dynamic_cast<LzListWidgetItem*>(item);
    if (!drawItem)
        return;

    LzTool* currTool = drawItem->getTool();
    if (currTool)
        LzToolManager::getInstance()->setCurrentTool(currTool);
}
