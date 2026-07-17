#ifndef CIMGENERATEDTOPOLOGYSCENEBUILDER_H
#define CIMGENERATEDTOPOLOGYSCENEBUILDER_H

#include "cim/query/CimGeneratedTopologyGraphSummary.h"

struct CimGeneratedTopologySceneBuildResult
{
    int generatedNodeCount = 0;
    int generatedLineCount = 0;

    bool isEmpty() const
    {
        return generatedNodeCount <= 0 && generatedLineCount <= 0;
    }
};

class CimdrawScene;

CimGeneratedTopologySceneBuildResult cimRenderGeneratedTopologyGraphSummary(
    CimdrawScene* targetScene,
    const CimGeneratedTopologyGraphSummary& graphSummary);

#endif
