#ifndef FRAMETOOLSELECTIONCONTROLLER_H
#define FRAMETOOLSELECTIONCONTROLLER_H

class QListWidgetItem;
class CimdrawScene;

class FrameToolSelectionController
{
public:
    void activateDrawTool(QListWidgetItem* item, CimdrawScene* scene) const;
};

#endif
