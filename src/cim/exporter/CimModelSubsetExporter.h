#ifndef CIMMODELSUBSETEXPORTER_H
#define CIMMODELSUBSETEXPORTER_H

#include "cim/validation/CimModelValidation.h"

#include <QDomDocument>
#include <QString>
#include <QStringList>

class CimIdIndex;
class CimModel;

struct CimModelSubsetExportOptions
{
    QStringList mrids;
    bool exportAllWhenMridsEmpty = true;
    bool includeReferencedObjects = false;
    bool requireGraphicBindingsForMappableObjects = false;
};

struct CimModelSubsetExportSummary
{
    int requestedCount = 0;
    int exportedObjectCount = 0;
    int missingObjectCount = 0;
    int validationErrorCount = 0;
    int validationWarningCount = 0;

    bool ok() const { return missingObjectCount == 0 && validationErrorCount == 0; }
};

struct CimModelSubsetExportResult
{
    bool success = false;
    QDomDocument document;
    CimModelSubsetExportSummary summary;
    CimModelValidationReport validationReport;
    QStringList missingMrids;

    QString xmlText(int indent = 2) const;
    QStringList reportLines() const;
};

CimModelSubsetExportResult exportCimModelSubsetToXml(
    const CimModel& model,
    const CimModelSubsetExportOptions& options = {},
    const CimIdIndex* graphicIndex = nullptr);

#endif // CIMMODELSUBSETEXPORTER_H
