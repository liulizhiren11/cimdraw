#ifndef CIMIMPORTCONTROLLER_H
#define CIMIMPORTCONTROLLER_H

#include "cim/importer/CimImportResult.h"

#include <QString>

class CimImportController
{
public:
    struct ExecuteResult
    {
        bool accepted = false;
        QString resolvedPath;
        QString validationError;
        CimImportResult importResult;
    };

    ExecuteResult executeImport(const QString& path) const;
};

#endif
