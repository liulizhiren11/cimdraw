#include "scene/LzSceneDocumentWorkbenchController.h"

#include "scene/LzSceneDocumentController.h"
#include "scene/LzSceneGroupController.h"

bool LzSceneDocumentWorkbenchController::load(LzScene* scene,
                                              const QString& filename,
                                              QString* inoutErrorMsg) const
{
    LzSceneDocumentController controller;
    return controller.load(scene, filename, inoutErrorMsg);
}

bool LzSceneDocumentWorkbenchController::toDomDocument(const QString& filename,
                                                       QDomDocument* dom,
                                                       QString* inoutErrorMsg) const
{
    LzSceneDocumentController controller;
    return controller.toDomDocument(filename, dom, inoutErrorMsg);
}

bool LzSceneDocumentWorkbenchController::save(LzScene* scene, const QString& filename) const
{
    LzSceneDocumentController controller;
    return controller.save(scene, filename);
}

FILE_TYPE LzSceneDocumentWorkbenchController::sceneType(FILE_TYPE currentType) const
{
    LzSceneDocumentController controller;
    return controller.sceneType(currentType);
}

void LzSceneDocumentWorkbenchController::setSceneType(FILE_TYPE* currentType, FILE_TYPE newType) const
{
    LzSceneDocumentController controller;
    controller.setSceneType(currentType, newType);
}

QGraphicsItemGroup* LzSceneDocumentWorkbenchController::createGroup(LzScene* scene,
                                                                    const QList<QGraphicsItem*>& items) const
{
    LzSceneGroupController controller;
    return controller.createGroup(scene, items);
}
