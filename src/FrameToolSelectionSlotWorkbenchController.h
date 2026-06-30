#ifndef FRAMETOOLSELECTIONSLOTWORKBENCHCONTROLLER_H
#define FRAMETOOLSELECTIONSLOTWORKBENCHCONTROLLER_H

class QListWidgetItem;
class LzCenterWidget;
class LzScene;

class FrameToolSelectionSlotWorkbenchController
{
public:
    LzScene* currentScene(LzCenterWidget* centerWidget) const;
    bool activateDrawTool(QListWidgetItem* item, LzCenterWidget* centerWidget) const;
};

#endif
