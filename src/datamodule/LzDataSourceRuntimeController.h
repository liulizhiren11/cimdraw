#ifndef LZDATASOURCERUNTIMECONTROLLER_H
#define LZDATASOURCERUNTIMECONTROLLER_H

#include <QList>

class LzIDataSource;

class LzDataSourceRuntimeController
{
public:
    void clearSources(QList<LzIDataSource*>& sources) const;
    void startAll(const QList<LzIDataSource*>& sources) const;
    void stopAll(const QList<LzIDataSource*>& sources) const;
};

#endif
