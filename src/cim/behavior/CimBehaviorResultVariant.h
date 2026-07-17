#ifndef CIMBEHAVIORRESULTVARIANT_H
#define CIMBEHAVIORRESULTVARIANT_H

#include "cim/behavior/CimBehaviorResult.h"

#include <QVariantMap>

QVariantMap cimBehaviorResultToVariantMap(const CimBehaviorResult& result);
CimBehaviorResult cimBehaviorResultFromVariantMap(const QVariantMap& map);
bool isCimBehaviorResultVariantMap(const QVariantMap& map);

#endif
