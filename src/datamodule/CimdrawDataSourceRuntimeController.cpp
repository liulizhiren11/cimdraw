#include "CimdrawDataSourceRuntimeController.h"

#include "CimdrawIDataSource.h"

#include <QDebug>

void CimdrawDataSourceRuntimeController::clearSources(QList<CimdrawIDataSource*>& sources) const
{
    stopAll(sources);
    for (CimdrawIDataSource* source : sources)
    {
        if (!source)
            continue;
        delete source;
    }
    sources.clear();
}

void CimdrawDataSourceRuntimeController::startAll(const QList<CimdrawIDataSource*>& sources) const
{
    for (CimdrawIDataSource* source : sources)
    {
        if (!source)
            continue;
        if (!source->start())
            qWarning() << "CimdrawDataSourceRuntimeController: start failed for" << source->typeId();
    }
}

void CimdrawDataSourceRuntimeController::stopAll(const QList<CimdrawIDataSource*>& sources) const
{
    for (CimdrawIDataSource* source : sources)
    {
        if (!source)
            continue;
        source->stop();
    }
}
