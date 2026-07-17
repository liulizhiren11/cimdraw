#ifndef FRAMESCENELIFECYCLECOORDINATOR_H
#define FRAMESCENELIFECYCLECOORDINATOR_H

#include "FrameCimWorkbenchController.h"
#include "FrameDocumentController.h"
#include "topology/ui/FrameTopologyController.h"

class CimdrawFrame;
class CimdrawCenterWidget;
class CimdrawScene;
class QLineEdit;

class FrameSceneLifecycleCoordinator
{
public:
    FrameDocumentController::NewSceneContext makeNewSceneContext(CimdrawFrame* frame,
                                                                 CimdrawCenterWidget* centerWidget) const;
    FrameDocumentController::OpenSceneContext makeOpenSceneContext(CimdrawFrame* frame) const;
    FrameTopologyController::GenerateContext makeTopologyContext(CimdrawFrame* frame,
                                                                 CimdrawScene* sourceScene) const;
    FrameCimWorkbenchController::ImportContext makeCimImportContext(CimdrawFrame* frame,
                                                                    QLineEdit* messageLineEdit) const;
};

#endif
