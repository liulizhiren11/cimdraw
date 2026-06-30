#ifndef FRAMEWORKBENCHUICONTROLLER_H
#define FRAMEWORKBENCHUICONTROLLER_H

#include <QGraphicsItem>
#include <QList>

class QPushButton;
class QTabWidget;
class QObject;
class LzItemController;
class LzScene;
class CimModelBrowserDock;

class FrameWorkbenchUiController
{
public:
    void handleCurrentTabChanged(QTabWidget* tabs,
                                 QPushButton* backgroundButton,
                                 CimModelBrowserDock* browserDock) const;
    QObject* handleCurrentObjectChanged(LzItemController* propertyController,
                                        const QList<QGraphicsItem*>& items,
                                        CimModelBrowserDock* browserDock) const;

private:
    QObject* syncSelectionToPropertyPanel(LzItemController* propertyController,
                                          const QList<QGraphicsItem*>& items) const;
    void syncCimBrowserSelection(CimModelBrowserDock* browserDock, LzScene* scene) const;
};

#endif
