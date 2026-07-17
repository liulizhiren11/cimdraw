#ifndef CIMBROWSERBRIDGE_H
#define CIMBROWSERBRIDGE_H

#include "cim/importer/CimImportResult.h"
#include "cim/query/CimGraphicQueryState.h"
#include "cim/query/CimGraphicVisualSummary.h"

class QMainWindow;
class CimModelBrowserDock;
class CimdrawScene;

class CimBrowserBridge
{
public:
    CimModelBrowserDock* ensureDock(QMainWindow* host, CimModelBrowserDock* currentDock) const;
    void updateDock(CimModelBrowserDock* dock,
                    const CimImportResult& result,
                    const CimGraphicVisualSummary& visualSummary) const;
};

#endif
