#ifndef LZDATASOURCEBUILDER_H
#define LZDATASOURCEBUILDER_H

#include "LzSceneResolver.h"

#include <QJsonObject>
#include <QList>
#include <QString>

class LzIDataSource;
class QObject;

class LzDataSourceBuilder
{
public:
    struct BuildResult
    {
        QList<LzIDataSource*> sources;
        QString error;
    };

    BuildResult buildSources(const QJsonObject& root,
                             LzSceneResolver resolver,
                             QObject* parent) const;
};

#endif
