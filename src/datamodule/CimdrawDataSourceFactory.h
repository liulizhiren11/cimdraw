#ifndef CIMDRAWDATASOURCEFACTORY_H
#define CIMDRAWDATASOURCEFACTORY_H

#include "CimdrawSceneResolver.h"

#include <QJsonObject>
#include <QString>

class CimdrawIDataSource;
class QObject;

class CimdrawDataSourceFactory
{
public:
    CimdrawIDataSource* createConfiguredSource(const QString& typeId,
                                          const QJsonObject& config,
                                          CimdrawSceneResolver resolver,
                                          QObject* parent) const;
};

#endif
