#ifndef FRAMETOOLSELECTIONCONTROLLER_H
#define FRAMETOOLSELECTIONCONTROLLER_H

class QListWidgetItem;
class LzScene;

class FrameToolSelectionController
{
public:
    void activateDrawTool(QListWidgetItem* item, LzScene* scene) const;
};

#endif
