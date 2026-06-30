#ifndef LZDATASOURCEFACTORY_H
#define LZDATASOURCEFACTORY_H

#include "LzSceneResolver.h"

#include <QJsonObject>
#include <QString>

class LzIDataSource;
class QObject;

class LzDataSourceFactory
{
public:
    LzIDataSource* createConfiguredSource(const QString& typeId,
                                          const QJsonObject& config,
                                          LzSceneResolver resolver,
                                          QObject* parent) const;
};

#endif
