#include "LzDataSourceRuntimeController.h"

#include "LzIDataSource.h"

#include <QDebug>

void LzDataSourceRuntimeController::clearSources(QList<LzIDataSource*>& sources) const
{
    stopAll(sources);
    for (LzIDataSource* source : sources)
    {
        if (!source)
            continue;
        delete source;
    }
    sources.clear();
}

void LzDataSourceRuntimeController::startAll(const QList<LzIDataSource*>& sources) const
{
    for (LzIDataSource* source : sources)
    {
        if (!source)
            continue;
        if (!source->start())
            qWarning() << "LzDataSourceRuntimeController: start failed for" << source->typeId();
    }
}

void LzDataSourceRuntimeController::stopAll(const QList<LzIDataSource*>& sources) const
{
    for (LzIDataSource* source : sources)
    {
        if (!source)
            continue;
        source->stop();
    }
}
