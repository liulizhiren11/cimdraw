#ifndef FRAMEWORKBENCHSTATECONTROLLER_H
#define FRAMEWORKBENCHSTATECONTROLLER_H

#include <QPointF>

class QComboBox;
class QLineEdit;
class QTabWidget;
class LzView;

enum class LzWiringSymbolStandard : int;

class FrameWorkbenchStateController
{
public:
    void syncWiringStandardCombo(QComboBox* combo, LzWiringSymbolStandard standard) const;
    void refreshAllWiringItemPaint(QTabWidget* tabs) const;
    void syncPointerPosition(QLineEdit* xLineEdit, QLineEdit* yLineEdit, const QPointF& point) const;
    LzView* currentViewFromTabs(QTabWidget* tabs) const;
};

#endif
