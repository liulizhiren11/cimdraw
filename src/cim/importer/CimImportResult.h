#ifndef CIMIMPORTRESULT_H
#define CIMIMPORTRESULT_H

#include "cim/model/CimModel.h"
#include "topology/PowerTopologyProjection.h"
#include "topology/PowerTopologyTypes.h"

#include <QHash>
#include <QString>
#include <QStringList>

struct CimImportSummary
{
    bool success = false;
    int objectCount = 0;
    int profileCount = 0;
    int errorCount = 0;
    int warningCount = 0;
    int logCount = 0;
    QStringList loadedProfiles;
    QStringList errors;
    QStringList warnings;
    QStringList logs;
    QHash<QString, int> classDistribution;

    bool hasErrors() const { return errorCount > 0; }
    bool hasWarnings() const { return warningCount > 0; }

    QStringList diagnosticLines() const
    {
        return {
            QStringLiteral("success=%1").arg(success),
            QStringLiteral("profiles=%1").arg(loadedProfiles.join(QStringLiteral(","))),
            QStringLiteral("errors=%1").arg(errors.join(QStringLiteral(" | "))),
            QStringLiteral("warnings=%1").arg(warnings.join(QStringLiteral(" | "))),
            QStringLiteral("logs=%1").arg(logs.join(QStringLiteral(" | "))),
            QStringLiteral("count=%1").arg(objectCount)
        };
    }

    QString diagnosticText() const
    {
        return diagnosticLines().join(QStringLiteral("\n"));
    }
};

struct CimImportResult
{
    bool success = false;
    CimModel model;
    PowerTopologyProjection topologyProjection;
    PowerTopologyAnalysisSnapshot topologySnapshot;
    QStringList logs;
    QStringList warnings;
    QStringList errors;
    QStringList loadedProfiles;

    CimImportSummary summary() const
    {
        CimImportSummary result;
        result.success = success;
        result.objectCount = model.objectCount();
        result.profileCount = loadedProfiles.size();
        result.errorCount = errors.size();
        result.warningCount = warnings.size();
        result.logCount = logs.size();
        result.loadedProfiles = loadedProfiles;
        result.errors = errors;
        result.warnings = warnings;
        result.logs = logs;
        result.classDistribution = model.classDistribution();
        return result;
    }
};

#endif // CIMIMPORTRESULT_H
