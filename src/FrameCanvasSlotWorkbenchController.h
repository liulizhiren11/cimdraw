#ifndef FRAMECANVASSLOTWORKBENCHCONTROLLER_H
#define FRAMECANVASSLOTWORKBENCHCONTROLLER_H

#include <QPointF>

class QComboBox;
class QLineEdit;
class QPushButton;
class QString;
class QWidget;
class CimdrawCenterWidget;
class CimdrawToolBoxManager;

enum class CimdrawWiringSymbolStandard : int;

class FrameCanvasSlotWorkbenchController
{
public:
    bool chooseAndApplyBackgroundColor(QWidget* host,
                                       CimdrawCenterWidget* centerWidget,
                                       QPushButton* button) const;
    bool chooseAndApplyBackgroundImage(QWidget* host, CimdrawCenterWidget* centerWidget) const;
    void syncWiringStandardUi(QComboBox* combo, CimdrawWiringSymbolStandard standard) const;
    bool applySelectedWiringStandard(QComboBox* combo, const QString& appConfigPath) const;
    void applyWiringStandardChange(QComboBox* combo,
                                   CimdrawToolBoxManager* toolBox,
                                   CimdrawCenterWidget* centerWidget,
                                   CimdrawWiringSymbolStandard standard) const;
    void refreshAllWiringItemPaint(CimdrawCenterWidget* centerWidget) const;
    void syncPointerPosition(QLineEdit* xLineEdit, QLineEdit* yLineEdit, const QPointF& point) const;
};

#endif
