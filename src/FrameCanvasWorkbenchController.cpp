#include "FrameCanvasWorkbenchController.h"

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QWidget>

#include "FrameCanvasAppearanceController.h"
#include "FrameWorkbenchStateController.h"
#include "CimdrawToolBoxManager.h"

bool FrameCanvasWorkbenchController::chooseAndApplyBackgroundColor(QWidget* host,
                                                                   QTabWidget* tabs,
                                                                   QPushButton* button) const
{
    FrameCanvasAppearanceController controller;
    return controller.chooseAndApplyBackgroundColor(host, tabs, button);
}

bool FrameCanvasWorkbenchController::chooseAndApplyBackgroundImage(QWidget* host, QTabWidget* tabs) const
{
    FrameCanvasAppearanceController controller;
    return controller.chooseAndApplyBackgroundImage(host, tabs);
}

void FrameCanvasWorkbenchController::syncWiringStandardUi(QComboBox* combo,
                                                          CimdrawWiringSymbolStandard standard) const
{
    FrameWorkbenchStateController controller;
    controller.syncWiringStandardCombo(combo, standard);
}

void FrameCanvasWorkbenchController::refreshAllWiringItemPaint(QTabWidget* tabs) const
{
    FrameWorkbenchStateController controller;
    controller.refreshAllWiringItemPaint(tabs);
}

void FrameCanvasWorkbenchController::applyWiringStandardChange(QComboBox* combo,
                                                               CimdrawToolBoxManager* toolBox,
                                                               QTabWidget* tabs,
                                                               CimdrawWiringSymbolStandard standard) const
{
    syncWiringStandardUi(combo, standard);
    if (toolBox)
        toolBox->refreshWiringToolIcons();
    refreshAllWiringItemPaint(tabs);
}

void FrameCanvasWorkbenchController::syncPointerPosition(QLineEdit* xLineEdit,
                                                         QLineEdit* yLineEdit,
                                                         const QPointF& point) const
{
    FrameWorkbenchStateController controller;
    controller.syncPointerPosition(xLineEdit, yLineEdit, point);
}
