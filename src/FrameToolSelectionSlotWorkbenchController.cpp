#include "FrameToolSelectionSlotWorkbenchController.h"

#include "FrameToolSelectionController.h"
#include "LzCenterWidget.h"
#include "LzView.h"

LzScene* FrameToolSelectionSlotWorkbenchController::currentScene(LzCenterWidget* centerWidget) const
{
    if (!centerWidget)
        return nullptr;

    LzView* view = centerWidget->getCurrentView();
    return view ? view->getScene() : nullptr;
}

bool FrameToolSelectionSlotWorkbenchController::activateDrawTool(QListWidgetItem* item,
                                                                 LzCenterWidget* centerWidget) const
{
    LzScene* scene = currentScene(centerWidget);
    if (!item || !scene)
        return false;

    FrameToolSelectionController controller;
    controller.activateDrawTool(item, scene);
    return true;
}
