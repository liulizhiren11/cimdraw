#ifndef CIMBEHAVIORQUERY_H
#define CIMBEHAVIORQUERY_H

#include "cim/behavior/CimBehaviorResult.h"

class QGraphicsItem;

class CimBehaviorQuery
{
public:
    CimBehaviorResult resultForItem(QGraphicsItem* item) const;
};

#endif
