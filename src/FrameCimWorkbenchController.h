#ifndef FRAMECIMWORKBENCHCONTROLLER_H
#define FRAMECIMWORKBENCHCONTROLLER_H

#include "cim/model/CimModel.h"

#include <QGraphicsItem>
#include <QHash>
#include <functional>

class QString;
class QWidget;
class QLineEdit;
class LzScene;
class LzView;
class CimModelBrowserDock;

class FrameCimWorkbenchController
{
public:
    struct ImportState
    {
        CimModelBrowserDock* browserDock = nullptr;
        CimModel lastImportedModel;
        QHash<QString, QGraphicsItem*> shapeByMrid;
        int generatedHelperLineCount = 0;
    };

    struct ImportContext
    {
        QWidget* host = nullptr;
        QLineEdit* messageLineEdit = nullptr;
        std::function<void(CimModelBrowserDock*)> onBrowserDockReady;
        std::function<LzView*()> createTargetView;
        std::function<void(const QString&)> renameCurrentTab;
        std::function<void()> afterSceneCreated;
    };

    QString defaultImportPath() const;
    QString chooseImportPath(QWidget* host) const;
    bool importFromPath(const QString& path, const ImportContext& context, ImportState* state) const;
    void activateObjectByMrid(const QString& mrid, LzScene* scene, const ImportState& state) const;

private:
    CimModelBrowserDock* ensureBrowserDock(QWidget* host,
                                           CimModelBrowserDock* currentDock,
                                           const std::function<void(CimModelBrowserDock*)>& onBrowserDockReady) const;
    void populateScene(LzScene* scene,
                       const CimModel& model,
                       QHash<QString, QGraphicsItem*>& shapeByMrid,
                       int& generatedHelperLineCount) const;
};

#endif
