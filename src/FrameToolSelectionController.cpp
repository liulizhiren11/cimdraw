#include "FrameToolSelectionController.h"

#include <QDebug>
#include <QListWidgetItem>

#include "CimdrawListWidgetItem.h"
#include "CimdrawScene.h"
#include "Tool/CimdrawTool.h"

void FrameToolSelectionController::activateDrawTool(QListWidgetItem* item, CimdrawScene* scene) const
{
    qDebug() << "onDrawToolChanged";
    if (!item || !scene || scene->getPaintState())
        return;

    auto* drawItem = dynamic_cast<CimdrawListWidgetItem*>(item);
    if (!drawItem)
        return;

    CimdrawTool* currTool = drawItem->getTool();
    if (currTool)
        CimdrawToolManager::getInstance()->setCurrentTool(currTool);
}
