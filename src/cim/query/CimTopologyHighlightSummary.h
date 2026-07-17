#ifndef CIMTOPOLOGYHIGHLIGHTSUMMARY_H
#define CIMTOPOLOGYHIGHLIGHTSUMMARY_H

#include <QStringList>

struct CimTopologyHighlightSummary
{
    bool canHighlight = false;
    QStringList nodeIds;
    QStringList edgeIds;

    bool isValid() const
    {
        return canHighlight && (!nodeIds.isEmpty() || !edgeIds.isEmpty());
    }
};

#endif
