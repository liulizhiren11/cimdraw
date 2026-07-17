#ifndef FRAMETOOLSELECTIONSLOTWORKBENCHCONTROLLER_H
#define FRAMETOOLSELECTIONSLOTWORKBENCHCONTROLLER_H

class QListWidgetItem;
class CimdrawCenterWidget;
class CimdrawScene;

class FrameToolSelectionSlotWorkbenchController
{
public:
    CimdrawScene* currentScene(CimdrawCenterWidget* centerWidget) const;
    bool activateDrawTool(QListWidgetItem* item, CimdrawCenterWidget* centerWidget) const;
};

#endif
