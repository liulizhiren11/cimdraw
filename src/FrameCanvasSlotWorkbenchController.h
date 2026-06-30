#ifndef FRAMECANVASSLOTWORKBENCHCONTROLLER_H
#define FRAMECANVASSLOTWORKBENCHCONTROLLER_H

#include <QPointF>

class QComboBox;
class QLineEdit;
class QPushButton;
class QString;
class QWidget;
class LzCenterWidget;
class LzToolBoxManager;

enum class LzWiringSymbolStandard : int;

class FrameCanvasSlotWorkbenchController
{
public:
    bool chooseAndApplyBackgroundColor(QWidget* host,
                                       LzCenterWidget* centerWidget,
                                       QPushButton* button) const;
    bool chooseAndApplyBackgroundImage(QWidget* host, LzCenterWidget* centerWidget) const;
    void syncWiringStandardUi(QComboBox* combo, LzWiringSymbolStandard standard) const;
    bool applySelectedWiringStandard(QComboBox* combo, const QString& appConfigPath) const;
    void applyWiringStandardChange(QComboBox* combo,
                                   LzToolBoxManager* toolBox,
                                   LzCenterWidget* centerWidget,
                                   LzWiringSymbolStandard standard) const;
    void refreshAllWiringItemPaint(LzCenterWidget* centerWidget) const;
    void syncPointerPosition(QLineEdit* xLineEdit, QLineEdit* yLineEdit, const QPointF& point) const;
};

#endif
