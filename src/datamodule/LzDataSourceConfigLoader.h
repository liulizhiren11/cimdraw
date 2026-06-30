#ifndef LZDATASOURCECONFIGLOADER_H
#define LZDATASOURCECONFIGLOADER_H

#include <QJsonObject>
#include <QString>

class LzDataSourceConfigLoader
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
