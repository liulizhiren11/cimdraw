#ifndef FRAMESCENELIFECYCLECOORDINATOR_H
#define FRAMESCENELIFECYCLECOORDINATOR_H

#include "FrameCimWorkbenchController.h"
#include "FrameDocumentController.h"
#include "topology/ui/FrameTopologyController.h"

class LzFrame;
class LzCenterWidget;
class LzScene;
class QLineEdit;

class FrameSceneLifecycleCoordinator
{
public:
    FrameDocumentController::NewSceneContext makeNewSceneContext(LzFrame* frame,
                                                                 LzCenterWidget* centerWidget) const;
    FrameDocumentController::OpenSceneContext makeOpenSceneContext(LzFrame* frame) const;
    FrameTopologyController::GenerateContext makeTopologyContext(LzFrame* frame,
                                                                 LzScene* sourceScene) const;
    FrameCimWorkbenchController::ImportContext makeCimImportContext(LzFrame* frame,
                                                                    QLineEdit* messageLineEdit) const;
};

#endif
