#ifndef FRAMEWORKBENCHSTATECONTROLLER_H
#define FRAMEWORKBENCHSTATECONTROLLER_H

#include <QPointF>

class QComboBox;
class QLineEdit;
class QTabWidget;
class CimdrawView;

enum class CimdrawWiringSymbolStandard : int;

class FrameWorkbenchStateController
{
public:
    void syncWiringStandardCombo(QComboBox* combo, CimdrawWiringSymbolStandard standard) const;
    void refreshAllWiringItemPaint(QTabWidget* tabs) const;
    void syncPointerPosition(QLineEdit* xLineEdit, QLineEdit* yLineEdit, const QPointF& point) const;
    CimdrawView* currentViewFromTabs(QTabWidget* tabs) const;
};

#endif
