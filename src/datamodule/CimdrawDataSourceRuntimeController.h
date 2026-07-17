#ifndef CIMDRAWDATASOURCERUNTIMECONTROLLER_H
#define CIMDRAWDATASOURCERUNTIMECONTROLLER_H

#include <QList>

class CimdrawIDataSource;

class CimdrawDataSourceRuntimeController
{
public:
    void clearSources(QList<CimdrawIDataSource*>& sources) const;
    void startAll(const QList<CimdrawIDataSource*>& sources) const;
    void stopAll(const QList<CimdrawIDataSource*>& sources) const;
};

#endif
