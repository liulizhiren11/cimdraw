#ifndef FRAMECIMWORKBENCHCONTROLLER_H
#define FRAMECIMWORKBENCHCONTROLLER_H

#include "cim/ui/CimSceneBuilder.h"
#include "cim/mapping/CimIdIndex.h"
#include "cim/model/CimModel.h"
#include "cim/query/CimGraphicQueryContext.h"
#include "cim/query/CimGraphicQueryState.h"
#include "cim/query/CimGraphicVisualSummary.h"

#include <QGraphicsItem>
#include <QHash>
#include <functional>

class QString;
class QWidget;
class QLineEdit;
class CimdrawScene;
class CimdrawView;
class CimModelBrowserDock;

class FrameCimWorkbenchController
{
public:
    struct ImportState
    {
        CimModelBrowserDock* browserDock = nullptr;
        CimModel lastImportedModel;
        QHash<QString, QGraphicsItem*> shapeByMrid;
        CimIdIndex idIndex;
        int generatedHelperLineCount = 0;

        CimGraphicQueryState graphicQueryState() const
        {
            return {{&shapeByMrid, &idIndex}, generatedHelperLineCount};
        }

        static CimGraphicVisualSummary buildGraphicVisualSummaryForQueryState(
            const CimModel& model,
            const CimGraphicQueryState& queryState = {});
        CimGraphicVisualSummary buildGraphicVisualSummary(const CimModel& model) const;
        void syncBrowserSelection(CimdrawScene* scene) const;

        void clearGraphicQueryState()
        {
            shapeByMrid.clear();
            idIndex = CimIdIndex();
            generatedHelperLineCount = 0;
        }

        void applyBuildResult(const CimSceneBuilder::BuildResult& buildResult)
        {
            const CimSceneBuilder::BuildResult::Summary summary = buildResult.summary();
            shapeByMrid = buildResult.shapeByMrid;
            idIndex = buildResult.idIndex;
            generatedHelperLineCount = summary.generatedHelperLineCount;
        }
    };

    struct ImportContext
    {
        QWidget* host = nullptr;
        QLineEdit* messageLineEdit = nullptr;
        std::function<void(CimModelBrowserDock*)> onBrowserDockReady;
        std::function<CimdrawView*()> createTargetView;
        std::function<void(const QString&)> renameCurrentTab;
        std::function<void()> afterSceneCreated;
    };

    QString defaultImportPath() const;
    QString chooseImportPath(QWidget* host) const;
    bool importFromPath(const QString& path, const ImportContext& context, ImportState* state) const;
    void activateObjectByMrid(const QString& mrid, CimdrawScene* scene, const ImportState& state) const;

private:
    CimModelBrowserDock* ensureBrowserDock(QWidget* host,
                                           CimModelBrowserDock* currentDock,
                                           const std::function<void(CimModelBrowserDock*)>& onBrowserDockReady) const;
    void populateScene(CimdrawScene* scene,
                       const CimModel& model,
                       ImportState* state) const;
};

#endif
