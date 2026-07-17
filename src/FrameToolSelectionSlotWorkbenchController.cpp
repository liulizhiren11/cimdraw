#include "FrameToolSelectionSlotWorkbenchController.h"

#include "CimdrawCenterWidget.h"
#include "FrameToolSelectionController.h"
#include "CimdrawView.h"

CimdrawScene* FrameToolSelectionSlotWorkbenchController::currentScene(CimdrawCenterWidget* centerWidget) const
{
    if (!centerWidget)
        return nullptr;

    CimdrawView* view = centerWidget->getCurrentView();
    return view ? view->getScene() : nullptr;
}

bool FrameToolSelectionSlotWorkbenchController::activateDrawTool(QListWidgetItem* item,
                                                                 CimdrawCenterWidget* centerWidget) const
{
    CimdrawScene* scene = currentScene(centerWidget);
    if (!item || !scene)
        return false;

    FrameToolSelectionController controller;
    controller.activateDrawTool(item, scene);
    return true;
}
