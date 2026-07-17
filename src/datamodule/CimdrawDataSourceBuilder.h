#ifndef CIMDRAWDATASOURCEBUILDER_H
#define CIMDRAWDATASOURCEBUILDER_H

#include "CimdrawSceneResolver.h"

#include <QJsonObject>
#include <QList>
#include <QString>

class CimdrawIDataSource;
class QObject;

class CimdrawDataSourceBuilder
{
public:
    struct BuildResult
    {
        QList<CimdrawIDataSource*> sources;
        QString error;
    };

    BuildResult buildSources(const QJsonObject& root,
                             CimdrawSceneResolver resolver,
                             QObject* parent) const;
};

#endif
