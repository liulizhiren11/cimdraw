#ifndef CIMIMPORTRESULT_H
#define CIMIMPORTRESULT_H

#include "cim/model/CimModel.h"

#include <QStringList>

struct CimImportResult
{
    bool success = false;
    CimModel model;
    QStringList logs;
    QStringList warnings;
    QStringList errors;
    QStringList loadedProfiles;
};

#endif // CIMIMPORTRESULT_H
