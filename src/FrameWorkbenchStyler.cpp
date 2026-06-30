#include "FrameWorkbenchStyler.h"

#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QToolBar>
#include <QWidget>

void FrameWorkbenchStyler::apply(QMainWindow* window) const
{
    if (!window)
        return;

    window->setObjectName(QStringLiteral("workbenchWindow"));
    window->setDockNestingEnabled(true);
    window->setDockOptions(QMainWindow::AllowNestedDocks
                           | QMainWindow::AllowTabbedDocks
                           | QMainWindow::AnimatedDocks);
    window->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    if (window->menuBar())
    {
        window->menuBar()->setObjectName(QStringLiteral("workbenchMenuBar"));
        window->menuBar()->setNativeMenuBar(false);
    }
    if (window->statusBar())
        window->statusBar()->setObjectName(QStringLiteral("workbenchStatusBar"));

    for (QToolBar* toolBar : window->findChildren<QToolBar*>())
    {
        toolBar->setMovable(false);
        toolBar->setFloatable(false);
        toolBar->setIconSize(QSize(20, 20));
        toolBar->setMinimumHeight(38);
    }

    if (auto* drawDock = window->findChild<QDockWidget*>(QStringLiteral("drawItemsDock")))
    {
        drawDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
        drawDock->setTitleBarWidget(new QWidget(drawDock));
        drawDock->setMinimumWidth(300);
    }
    if (auto* propertyDock = window->findChild<QDockWidget*>(QStringLiteral("propertyDock")))
        propertyDock->setMinimumWidth(300);

    window->setStyleSheet(QStringLiteral(
        "QMainWindow#workbenchWindow { background: #f5f7fb; }"
        "QMenuBar#workbenchMenuBar { background: #eef2f7; color: #1f2937; border-bottom: 1px solid #d7dde8; spacing: 10px; padding: 6px 12px; }"
        "QMenuBar#workbenchMenuBar::item { background: transparent; padding: 7px 10px; border-radius: 5px; }"
        "QMenuBar#workbenchMenuBar::item:selected { background: #dfe7f3; }"
        "QToolBar { background: #eef2f7; border: none; border-bottom: 1px solid #d7dde8; spacing: 6px; padding: 6px 10px; }"
        "QToolBar::separator { background: #cfd8e3; width: 1px; margin: 6px 8px; }"
        "QToolButton { color: #1f2937; border: 1px solid transparent; border-radius: 5px; padding: 7px 8px; background: transparent; }"
        "QToolButton:hover { background: #dfe7f3; border-color: #cfd8e3; }"
        "QToolButton:pressed { background: #d4e1f4; border-color: #b7c9ea; }"
        "QDockWidget { background: #f7f9fc; color: #1f1f1f; border: 1px solid #d7dde8; }"
        "QDockWidget::title { text-align: left; background: #eef2f7; padding: 8px 12px; border-bottom: 1px solid #d7dde8; }"
        "QTabWidget#editorTabs::pane { border: none; background: #ffffff; }"
        "QTabWidget#editorTabs QTabBar::tab { background: #e7ecf3; color: #334155; padding: 10px 18px; border: none; border-right: 1px solid #d7dde8; min-width: 144px; }"
        "QTabWidget#editorTabs QTabBar::tab:selected { background: #ffffff; border-top: 2px solid #007acc; padding-top: 6px; }"
        "QTabWidget#editorTabs QTabBar::tab:hover:!selected { background: #dde5ef; }"
        "QToolButton#editorTabCornerButton { margin: 4px; padding: 5px; }"
        "QStatusBar#workbenchStatusBar { background: #007acc; color: white; border-top: 1px solid #0068ad; min-height: 26px; }"
        "QStatusBar#workbenchStatusBar::item { border: none; }"
        "QLineEdit#statusCoordEdit, QLineEdit#statusMessageEdit { background: transparent; color: white; border: none; padding: 0 6px; }"
        "QDockWidget QListWidget, QDockWidget QTextEdit { background: #ffffff; color: #1f1f1f; border: 1px solid #e5e7eb; border-radius: 4px; }"
        "QDockWidget QTabWidget::pane { border: 1px solid #d7dde8; background: #ffffff; top: -1px; }"
        "QDockWidget QTabBar::tab { background: #eef2f7; padding: 8px 13px; border: 1px solid #d7dde8; border-bottom: none; margin-right: 2px; }"
        "QDockWidget QTabBar::tab:selected { background: #ffffff; border-top: 2px solid #007acc; }"));
}
