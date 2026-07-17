#ifndef CIMGRAPHICVISUALSUMMARY_H
#define CIMGRAPHICVISUALSUMMARY_H

#include "cim/query/CimGraphicObjectSummary.h"

#include <QHash>
#include <QSet>

class QString;

struct CimGraphicVisualSummary
{
    QSet<QString> visualizedMrids;
    QHash<QString, CimGraphicObjectSummary> objectSummariesByMrid;
    QHash<QString, CimGraphicObjectVisualState> objectVisualStatesByMrid;
    QHash<QString, CimBehaviorResult> behaviorResultsByMrid;
    int totalObjects = 0;
    int mappableObjects = 0;
    int visualizedObjects = 0;
    int pendingObjects = 0;
    int behaviorAvailableObjects = 0;
    int conductiveObjects = 0;
    int operableObjects = 0;
    int energizedObjects = 0;
    int unmappedDeviceObjects = 0;
    int semanticObjects = 0;
    int layoutObjects = 0;
    int metadataObjects = 0;
    int abstractObjects = 0;
    int unmappedObjects = 0;
    QSet<QString> unmappedDeviceClasses;
    QSet<QString> unmappedOtherClasses;
    int acLineSegmentObjects = 0;
    int visualizedAcLineSegments = 0;
    int generatedHelperLineCount = 0;
};

#endif
