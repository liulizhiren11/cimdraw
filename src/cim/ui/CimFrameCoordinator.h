#ifndef CIMFRAMECOORDINATOR_H
#define CIMFRAMECOORDINATOR_H

#include "cim/importer/CimImportResult.h"

#include <QGraphicsItem>
#include <QHash>
#include <functional>

class QString;
class QWidget;
class CimdrawScene;
class CimdrawView;
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

    struct ImportPresentation
    {
        QString statusMessage;
        QString detailText;

        bool hasDetails() const { return !detailText.trimmed().isEmpty(); }
    };

    struct ImportExecutionContext
    {
        QWidget* host = nullptr;
        QLineEdit* messageLineEdit = nullptr;
        CimModelBrowserDock* browserDock = nullptr;
        std::function<CimdrawView*()> createTargetView;
        std::function<void(const QString&)> renameCurrentTab;
        std::function<void(CimdrawScene*, const CimModel&)> populateScene;
        std::function<void()> afterSceneCreated;
    };

    ImportDialogResult chooseImportPath(QWidget* host, const QString& defaultPath) const;
    static ImportPresentation buildImportPresentation(const CimImportResult& result);
    bool applyImportResult(const CimImportResult& result, const ImportExecutionContext& context) const;
};

#endif
