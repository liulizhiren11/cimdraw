#include "CimBrowserBridge.h"

#include <QDockWidget>
#include <QMainWindow>

#include "LzScene.h"
#include "LzView.h"
#include "cim/ui/CimModelBrowserDock.h"
#include "cim/ui/CimSceneBuilder.h"

CimModelBrowserDock* CimBrowserBridge::ensureDock(QMainWindow* host, CimModelBrowserDock* currentDock) const
{
    if (!host)
        return currentDock;
    if (currentDock)
        return currentDock;

    auto* dock = new CimModelBrowserDock(host);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    host->addDockWidget(Qt::RightDockWidgetArea, dock);
    if (QDockWidget* propertyDock = host->findChild<QDockWidget*>(QStringLiteral("propertyDock")))
        host->tabifyDockWidget(propertyDock, dock);
    dock->raise();
    return dock;
}

void CimBrowserBridge::updateDock(CimModelBrowserDock* dock,
                                  const CimImportResult& result,
                                  const QHash<QString, QGraphicsItem*>& shapeByMrid) const
{
    updateDock(dock, result, shapeByMrid, 0);
}

void CimBrowserBridge::updateDock(CimModelBrowserDock* dock,
                                  const CimImportResult& result,
                                  const QHash<QString, QGraphicsItem*>& shapeByMrid,
                                  int generatedHelperLineCount) const
{
    if (!dock)
        return;

    QSet<QString> visualizedMrids;
    for (auto it = shapeByMrid.constBegin(); it != shapeByMrid.constEnd(); ++it)
    {
        if (it.value())
            visualizedMrids.insert(it.key());
    }

    dock->setImportResult(result.model,
                          result.loadedProfiles,
                          result.logs,
                          result.warnings,
                          result.errors,
                          visualizedMrids,
                          generatedHelperLineCount);
    dock->show();
    dock->raise();
}

void CimBrowserBridge::activateSceneItemByMrid(const QString& mrid,
                                               LzScene* scene,
                                               const QHash<QString, QGraphicsItem*>& shapeByMrid) const
{
    if (mrid.trimmed().isEmpty() || !scene)
        return;

    QGraphicsItem* item = shapeByMrid.value(mrid, nullptr);
    if (!item || item->scene() != scene)
        return;

    scene->cleanSelection();
    scene->addSelection(item);
    if (LzView* view = scene->getView())
        view->centerOn(item);
}

void CimBrowserBridge::syncDockSelectionFromScene(CimModelBrowserDock* dock, LzScene* scene) const
{
    if (!dock || !scene)
        return;

    const QList<QGraphicsItem*> selections = scene->getSelections();
    if (selections.size() != 1)
        return;

    const QString mrid = selections.first()->data(CimSceneBuilder::CimMridDataKey).toString();
    if (!mrid.isEmpty())
        dock->selectObjectByMrid(mrid);
}
