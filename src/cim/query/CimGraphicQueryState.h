#ifndef CIMGRAPHICQUERYSTATE_H
#define CIMGRAPHICQUERYSTATE_H

#include "cim/query/CimGraphicQueryContext.h"

struct CimGraphicQueryState
{
    CimGraphicQueryContext queryContext;
    int generatedHelperLineCount = 0;
};

#endif
