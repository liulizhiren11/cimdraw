#ifndef CIMBROWSERBRIDGE_H
#define CIMBROWSERBRIDGE_H

#include "cim/importer/CimImportResult.h"

#include <QGraphicsItem>
#include <QHash>
#include <QSet>

class QMainWindow;
class CimModelBrowserDock;
class LzScene;

class CimBrowserBridge
{
public:
    CimModelBrowserDock* ensureDock(QMainWindow* host, CimModelBrowserDock* currentDock) const;
    void updateDock(CimModelBrowserDock* dock,
                    const CimImportResult& result,
                    const QHash<QString, QGraphicsItem*>& shapeByMrid) const;
    void updateDock(CimModelBrowserDock* dock,
                    const CimImportResult& result,
                    const QHash<QString, QGraphicsItem*>& shapeByMrid = {},
                    int generatedHelperLineCount = 0) const;
    void activateSceneItemByMrid(const QString& mrid,
                                 LzScene* scene,
                                 const QHash<QString, QGraphicsItem*>& shapeByMrid) const;
    void syncDockSelectionFromScene(CimModelBrowserDock* dock, LzScene* scene) const;
};

#endif
