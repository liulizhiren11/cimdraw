#include "FrameCanvasSlotWorkbenchController.h"

#include <QComboBox>
#include <QLineEdit>
#include <QTabWidget>

#include "CimdrawAppConfig.h"
#include "CimdrawCenterWidget.h"
#include "FrameCanvasWorkbenchController.h"
#include "wiring/base/CimdrawWiringSymbolStyle.h"

namespace {

QTabWidget* tabWidgetFromCenterWidget(CimdrawCenterWidget* centerWidget)
{
    return centerWidget ? centerWidget->getTabWidget() : nullptr;
}

}

bool FrameCanvasSlotWorkbenchController::chooseAndApplyBackgroundColor(QWidget* host,
                                                                       CimdrawCenterWidget* centerWidget,
                                                                       QPushButton* button) const
{
    FrameCanvasWorkbenchController controller;
    return controller.chooseAndApplyBackgroundColor(host, tabWidgetFromCenterWidget(centerWidget), button);
}

bool FrameCanvasSlotWorkbenchController::chooseAndApplyBackgroundImage(QWidget* host,
                                                                       CimdrawCenterWidget* centerWidget) const
{
    FrameCanvasWorkbenchController controller;
    return controller.chooseAndApplyBackgroundImage(host, tabWidgetFromCenterWidget(centerWidget));
}

void FrameCanvasSlotWorkbenchController::syncWiringStandardUi(QComboBox* combo,
                                                              CimdrawWiringSymbolStandard standard) const
{
    FrameCanvasWorkbenchController controller;
    controller.syncWiringStandardUi(combo, standard);
}

bool FrameCanvasSlotWorkbenchController::applySelectedWiringStandard(QComboBox* combo,
                                                                     const QString& appConfigPath) const
{
    if (!combo || combo->currentIndex() < 0)
        return false;

    const auto standard = static_cast<CimdrawWiringSymbolStandard>(combo->currentData().toInt());
    CimdrawWiringSymbolStyle::instance().setStandard(standard);
    CimdrawAppConfig::saveWiringSymbolStandard(appConfigPath, standard);
    return true;
}

void FrameCanvasSlotWorkbenchController::applyWiringStandardChange(QComboBox* combo,
                                                                   CimdrawToolBoxManager* toolBox,
                                                                   CimdrawCenterWidget* centerWidget,
                                                                   CimdrawWiringSymbolStandard standard) const
{
    FrameCanvasWorkbenchController controller;
    controller.applyWiringStandardChange(combo, toolBox, tabWidgetFromCenterWidget(centerWidget), standard);
}

void FrameCanvasSlotWorkbenchController::refreshAllWiringItemPaint(CimdrawCenterWidget* centerWidget) const
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
