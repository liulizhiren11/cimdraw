#include "FrameSceneLifecycleCoordinator.h"

#include <QLineEdit>

#include "cim/ui/CimModelBrowserDock.h"
#include "CimdrawCenterWidget.h"
#include "CimdrawFrame.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"

FrameDocumentController::NewSceneContext FrameSceneLifecycleCoordinator::makeNewSceneContext(
    CimdrawFrame* frame,
    CimdrawCenterWidget* centerWidget) const
{
    FrameDocumentController::NewSceneContext context;
    context.centerWidget = centerWidget;
    context.afterCreate = [frame](CimdrawView* view, CimdrawScene* scene) {
        if (!frame || !view || !scene)
            return;
        QObject::connect(view, SIGNAL(editChanged()), frame, SLOT(onUpdateRedoUndo()));
        QObject::connect(view, SIGNAL(mouseMove(const QPointF&)), frame, SLOT(onUpdatePostion(const QPointF&)));
        QObject::connect(scene, SIGNAL(currentObjectChanged(QList<QGraphicsItem*>)),
                         frame, SLOT(onCurrentObjectChanged(QList<QGraphicsItem*>)));
        frame->onUpdateFileActions();
    };
    context.renameCurrentTab = [frame](const QString& title) {
        if (frame)
            frame->setTapWidgetName(title);
    };
    return context;
}

FrameDocumentController::OpenSceneContext FrameSceneLifecycleCoordinator::makeOpenSceneContext(CimdrawFrame* frame) const
{
    FrameDocumentController::OpenSceneContext context;
    context.host = frame;
    context.createEmptyScene = [frame]() { return frame ? frame->newPowerScene() : nullptr; };
    context.renameCurrentTab = [frame](const QString& title) {
        if (frame)
            frame->setTapWidgetName(title);
    };
    context.afterOpen = [frame]() {
        if (!frame)
            return;
        frame->onUpdateFileActions();
        frame->onUpdateRedoUndo();
    };
    return context;
}

FrameTopologyController::GenerateContext FrameSceneLifecycleCoordinator::makeTopologyContext(
    CimdrawFrame* frame,
    CimdrawScene* sourceScene) const
{
    FrameTopologyController::GenerateContext context;
    context.host = frame;
    context.sourceScene = sourceScene;
    context.createTargetView = [frame]() { return frame ? frame->newPowerScene() : nullptr; };
    context.renameCurrentTab = [frame](const QString& title) {
        if (frame)
            frame->setTapWidgetName(title);
    };
    context.onGenerated = [frame]() {
        if (frame)
            frame->onUpdateFileActions();
    };
    return context;
}

FrameCimWorkbenchController::ImportContext FrameSceneLifecycleCoordinator::makeCimImportContext(
    CimdrawFrame* frame,
    QLineEdit* messageLineEdit) const
{
    FrameCimWorkbenchController::ImportContext context;
    context.host = frame;
    context.messageLineEdit = messageLineEdit;
    context.onBrowserDockReady = [frame](CimModelBrowserDock* dock) {
        if (!frame || !dock)
            return;
        QObject::connect(dock, &CimModelBrowserDock::objectActivated,
                         frame, &CimdrawFrame::onCimBrowserObjectActivated);
    };
    context.createTargetView = [frame]() { return frame ? frame->newPowerScene() : nullptr; };
    context.renameCurrentTab = [frame](const QString& title) {
        if (frame)
            frame->setTapWidgetName(title);
    };
    context.afterSceneCreated = [frame]() {
        if (frame)
            frame->onUpdateFileActions();
    };
    return context;
}
