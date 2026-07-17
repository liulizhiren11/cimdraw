#ifndef CIMMODELVALIDATION_H
#define CIMMODELVALIDATION_H

#include <QString>
#include <QStringList>
#include <QVector>

class CimIdIndex;
class CimModel;

enum class CimValidationSeverity
{
    Info = 0,
    Warning,
    Error,
};

enum class CimValidationIssueCode
{
    EmptyModel = 0,
    InvalidObject,
    MissingReference,
    DanglingGraphicBinding,
    MissingGraphicBinding,
};

struct CimValidationIssue
{
    CimValidationSeverity severity = CimValidationSeverity::Info;
    CimValidationIssueCode code = CimValidationIssueCode::EmptyModel;
    QString objectMrid;
    QString className;
    QString relationName;
    QString targetMrid;
    QString graphicItemId;
    QString message;
};

struct CimModelValidationOptions
{
    bool reportEmptyModel = true;
    bool requireGraphicBindingsForMappableObjects = false;
};

struct CimModelValidationSummary
{
    int objectCount = 0;
    int referenceCount = 0;
    int mappableObjectCount = 0;
    int graphicBindingCount = 0;
    int missingGraphicBindingCount = 0;
    int danglingGraphicBindingCount = 0;
    int errorCount = 0;
    int warningCount = 0;
    int infoCount = 0;

    bool ok() const { return errorCount == 0; }
};

struct CimModelValidationReport
{
    CimModelValidationSummary summary;
    QVector<CimValidationIssue> issues;

    bool ok() const { return summary.ok(); }
    QStringList messages() const;
};

CimModelValidationReport validateCimModelStructure(
    const CimModel& model,
    const CimIdIndex* graphicIndex = nullptr,
    const CimModelValidationOptions& options = {});

#endif // CIMMODELVALIDATION_H
