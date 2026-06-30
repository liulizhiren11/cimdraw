#include "FrameDockBuilder.h"

#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "LzFrame.h"
#include "LzItemController.h"
#include "LzToolBoxManager.h"

FrameDockBuilder::DrawItemsDockResult FrameDockBuilder::createDrawItemsDock(QMainWindow* host, LzFrame* frame) const
{
    DrawItemsDockResult result;
    if (!host || !frame)
        return result;

    QWidget* mainWidget = new QWidget(host);
    mainWidget->setObjectName(QStringLiteral("drawItemsSidebarContainer"));
    QVBoxLayout* layout = new QVBoxLayout(mainWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    result.toolBox = new LzToolBoxManager(frame);
    layout->addWidget(result.toolBox->getWidget());

    result.dockWidget = new QDockWidget(host);
    result.dockWidget->setObjectName(QStringLiteral("drawItemsDock"));
    result.dockWidget->setWidget(mainWidget);
    result.dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    result.dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    result.dockWidget->setWindowTitle(QObject::tr("图元"));
    result.dockWidget->setMinimumWidth(300);
    host->addDockWidget(Qt::LeftDockWidgetArea, result.dockWidget);
    return result;
}

FrameDockBuilder::PropertyDockResult FrameDockBuilder::createPropertyDock(QMainWindow* host, LzFrame* frame) const
{
    PropertyDockResult result;
    if (!host || !frame)
        return result;

    result.tabWidget = new QTabWidget(host);
    result.tabWidget->setDocumentMode(true);
    result.controller = new LzItemController(frame);
    result.tabWidget->addTab(result.controller, QObject::tr("图元属性"));

    result.dockWidget = new QDockWidget(host);
    result.dockWidget->setObjectName(QStringLiteral("propertyDock"));
    result.dockWidget->setWidget(result.tabWidget);
    result.dockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    result.dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    result.dockWidget->setWindowTitle(QObject::tr("属性"));
    result.dockWidget->setMinimumWidth(300);
    host->addDockWidget(Qt::RightDockWidgetArea, result.dockWidget);
    return result;
}
