#include "FrameCanvasSlotWorkbenchController.h"

#include <QComboBox>
#include <QLineEdit>
#include <QTabWidget>

#include "LzAppConfig.h"
#include "FrameCanvasWorkbenchController.h"
#include "LzCenterWidget.h"
#include "wiring/base/LzWiringSymbolStyle.h"

namespace {

QTabWidget* tabWidgetFromCenterWidget(LzCenterWidget* centerWidget)
{
    return centerWidget ? centerWidget->getTabWidget() : nullptr;
}

}

bool FrameCanvasSlotWorkbenchController::chooseAndApplyBackgroundColor(QWidget* host,
                                                                       LzCenterWidget* centerWidget,
                                                                       QPushButton* button) const
{
    FrameCanvasWorkbenchController controller;
    return controller.chooseAndApplyBackgroundColor(host, tabWidgetFromCenterWidget(centerWidget), button);
}

bool FrameCanvasSlotWorkbenchController::chooseAndApplyBackgroundImage(QWidget* host,
                                                                       LzCenterWidget* centerWidget) const
{
    FrameCanvasWorkbenchController controller;
    return controller.chooseAndApplyBackgroundImage(host, tabWidgetFromCenterWidget(centerWidget));
}

void FrameCanvasSlotWorkbenchController::syncWiringStandardUi(QComboBox* combo,
                                                              LzWiringSymbolStandard standard) const
{
    FrameCanvasWorkbenchController controller;
    controller.syncWiringStandardUi(combo, standard);
}

bool FrameCanvasSlotWorkbenchController::applySelectedWiringStandard(QComboBox* combo,
                                                                     const QString& appConfigPath) const
{
    if (!combo || combo->currentIndex() < 0)
        return false;

    const auto standard = static_cast<LzWiringSymbolStandard>(combo->currentData().toInt());
    LzWiringSymbolStyle::instance().setStandard(standard);
    LzAppConfig::saveWiringSymbolStandard(appConfigPath, standard);
    return true;
}

void FrameCanvasSlotWorkbenchController::applyWiringStandardChange(QComboBox* combo,
                                                                   LzToolBoxManager* toolBox,
                                                                   LzCenterWidget* centerWidget,
                                                                   LzWiringSymbolStandard standard) const
{
    FrameCanvasWorkbenchController controller;
    controller.applyWiringStandardChange(combo, toolBox, tabWidgetFromCenterWidget(centerWidget), standard);
}

void FrameCanvasSlotWorkbenchController::refreshAllWiringItemPaint(LzCenterWidget* centerWidget) const
{
    FrameCanvasWorkbenchController controller;
    controller.refreshAllWiringItemPaint(tabWidgetFromCenterWidget(centerWidget));
}

void FrameCanvasSlotWorkbenchController::syncPointerPosition(QLineEdit* xLineEdit,
                                                             QLineEdit* yLineEdit,
                                                             const QPointF& point) const
{
    FrameCanvasWorkbenchController controller;
    controller.syncPointerPosition(xLineEdit, yLineEdit, point);
}
