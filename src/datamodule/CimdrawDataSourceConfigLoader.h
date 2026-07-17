#ifndef CIMDRAWDATASOURCECONFIGLOADER_H
#define CIMDRAWDATASOURCECONFIGLOADER_H

#include <QJsonObject>
#include <QString>

class CimdrawDataSourceConfigLoader
{
public:
    struct LoadResult
    {
        bool success = false;
        QString error;
        QJsonObject root;
    };

    LoadResult loadRootObject(const QString& absolutePath) const;
};

#endif
