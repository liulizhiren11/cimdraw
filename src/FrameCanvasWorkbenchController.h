#ifndef FRAMECANVASWORKBENCHCONTROLLER_H
#define FRAMECANVASWORKBENCHCONTROLLER_H

#include <QPointF>

class QComboBox;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QWidget;
class LzToolBoxManager;

enum class LzWiringSymbolStandard : int;

class FrameCanvasWorkbenchController
{
public:
    bool chooseAndApplyBackgroundColor(QWidget* host, QTabWidget* tabs, QPushButton* button) const;
    bool chooseAndApplyBackgroundImage(QWidget* host, QTabWidget* tabs) const;

    void syncWiringStandardUi(QComboBox* combo, LzWiringSymbolStandard standard) const;
    void refreshAllWiringItemPaint(QTabWidget* tabs) const;
    void applyWiringStandardChange(QComboBox* combo,
                                   LzToolBoxManager* toolBox,
                                   QTabWidget* tabs,
                                   LzWiringSymbolStandard standard) const;
    void syncPointerPosition(QLineEdit* xLineEdit, QLineEdit* yLineEdit, const QPointF& point) const;
};

#endif
