#include "scene/CimdrawSceneDocumentWorkbenchController.h"

#include "scene/CimdrawSceneDocumentController.h"
#include "scene/CimdrawSceneGroupController.h"

bool CimdrawSceneDocumentWorkbenchController::load(CimdrawScene* scene,
                                              const QString& filename,
                                              QString* inoutErrorMsg) const
{
    CimdrawSceneDocumentController controller;
    return controller.load(scene, filename, inoutErrorMsg);
}

bool CimdrawSceneDocumentWorkbenchController::toDomDocument(const QString& filename,
                                                       QDomDocument* dom,
                                                       QString* inoutErrorMsg) const
{
    CimdrawSceneDocumentController controller;
    return controller.toDomDocument(filename, dom, inoutErrorMsg);
}

bool CimdrawSceneDocumentWorkbenchController::save(CimdrawScene* scene, const QString& filename) const
{
    CimdrawSceneDocumentController controller;
    return controller.save(scene, filename);
}

FILE_TYPE CimdrawSceneDocumentWorkbenchController::sceneType(FILE_TYPE currentType) const
{
    CimdrawSceneDocumentController controller;
    return controller.sceneType(currentType);
}

void CimdrawSceneDocumentWorkbenchController::setSceneType(FILE_TYPE* currentType, FILE_TYPE newType) const
{
    CimdrawSceneDocumentController controller;
    controller.setSceneType(currentType, newType);
}

QGraphicsItemGroup* CimdrawSceneDocumentWorkbenchController::createGroup(CimdrawScene* scene,
                                                                    const QList<QGraphicsItem*>& items) const
{
    CimdrawSceneGroupController controller;
    return controller.createGroup(scene, items);
}
