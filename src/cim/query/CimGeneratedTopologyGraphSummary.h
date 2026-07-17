#ifndef CIMGENERATEDTOPOLOGYGRAPHSUMMARY_H
#define CIMGENERATEDTOPOLOGYGRAPHSUMMARY_H

#include "cim/query/CimGeneratedTopologyRelationSummary.h"
#include "cim/query/CimGeneratedTopologySourceSummary.h"

#include <QPointF>
#include <QVector>

struct CimGeneratedTopologyGraphSummary
{
    QVector<CimGeneratedTopologySourceSummary> nodeSummaries;
    QVector<CimGeneratedTopologyRelationSummary> relationSummaries;
    QPointF layoutOrigin = QPointF(120.0, 120.0);
    QPointF nodeSpacing = QPointF(220.0, 160.0);

    int nodeColumnCount() const
    {
        const int count = nodeSummaries.size();
        if (count <= 1)
            return 1;

        int columns = 1;
        while (columns * columns < count)
            ++columns;
        return columns;
    }

    QPointF nodeScenePos(int index) const
    {
        if (index <= 0)
            return layoutOrigin;

        const int columns = nodeColumnCount();
        const int row = index / columns;
        const int col = index % columns;
        return QPointF(layoutOrigin.x() + col * nodeSpacing.x(),
                       layoutOrigin.y() + row * nodeSpacing.y());
    }

    bool isEmpty() const
    {
        return nodeSummaries.isEmpty();
    }
};

#endif
