#ifndef CIMGRAPHICQUERYCONTEXT_H
#define CIMGRAPHICQUERYCONTEXT_H

#include <QGraphicsItem>
#include <QHash>

class QString;
class CimIdIndex;

struct CimGraphicQueryContext
{
    const QHash<QString, QGraphicsItem*>* shapeByMrid = nullptr;
    const CimIdIndex* idIndex = nullptr;
};

#endif
