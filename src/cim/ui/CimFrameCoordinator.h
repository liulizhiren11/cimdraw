#ifndef CIMFRAMECOORDINATOR_H
#define CIMFRAMECOORDINATOR_H

#include "cim/importer/CimImportResult.h"

#include <QGraphicsItem>
#include <QHash>
#include <functional>

class QString;
class QWidget;
class LzScene;
class LzView;
class CimModelBrowserDock;
class QLineEdit;

class CimFrameCoordinator
{
public:
    struct ImportDialogResult
    {
        bool accepted = false;
        QString selectedPath;
    };

    struct ImportExecutionContext
    {
        QWidget* host = nullptr;
        QLineEdit* messageLineEdit = nullptr;
        CimModelBrowserDock* browserDock = nullptr;
        std::function<LzView*()> createTargetView;
        std::function<void(const QString&)> renameCurrentTab;
        std::function<void(LzScene*, const CimModel&)> populateScene;
        std::function<void()> afterSceneCreated;
    };

    ImportDialogResult chooseImportPath(QWidget* host, const QString& defaultPath) const;
    bool applyImportResult(const CimImportResult& result, const ImportExecutionContext& context) const;
};

#endif
