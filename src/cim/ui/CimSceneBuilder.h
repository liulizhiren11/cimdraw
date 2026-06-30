#ifndef CIMSCENEBUILDER_H
#define CIMSCENEBUILDER_H

#include <QGraphicsItem>
#include <QHash>

class LzScene;
class CimModel;

class CimSceneBuilder
{
public:
    static constexpr int CimMridDataKey = 1001;
    static constexpr int CimGeneratedVisualHelperDataKey = 1002;

    struct BuildResult
    {
        QHash<QString, QGraphicsItem*> shapeByMrid;
        int generatedHelperLineCount = 0;
    };

    BuildResult populateSceneFromCimModel(LzScene* scene, const CimModel& model) const;
};

#endif
