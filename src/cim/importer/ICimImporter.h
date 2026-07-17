#ifndef ICIMIMPORTER_H
#define ICIMIMPORTER_H

#include "CimImportResult.h"

#include <QString>

class ICimImporter
{
public:
    virtual ~ICimImporter() = default;

    virtual CimImportResult importFromPath(const QString& path) const = 0;
};

#endif // ICIMIMPORTER_H
