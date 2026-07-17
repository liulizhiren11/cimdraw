#ifndef CIMSCENEBUILDER_H
#define CIMSCENEBUILDER_H

#include <QGraphicsItem>
#include <QHash>

#include "cim/mapping/CimIdIndex.h"
#include "cim/query/CimGraphicQueryState.h"

class CimdrawScene;
class CimModel;

class CimSceneBuilder
{
public:
    static constexpr int CimMridDataKey = 1001;
    static constexpr int CimGeneratedVisualHelperDataKey = 1002;
    static constexpr int CimGraphicItemIdDataKey = 1003;

    struct BuildResult
    {
        struct Summary
        {
            int sceneItemCount = 0;
            int bindingCount = 0;
            int generatedHelperLineCount = 0;

            bool isEmpty() const
            {
                return sceneItemCount == 0
                    && bindingCount == 0
                    && generatedHelperLineCount == 0;
            }
        };

        QHash<QString, QGraphicsItem*> shapeByMrid;
        CimIdIndex idIndex;
        int generatedHelperLineCount = 0;

        bool isEmpty() const { return summary().isEmpty(); }
        int sceneItemCount() const { return shapeByMrid.size(); }
        int bindingCount() const { return idIndex.bindingCount(); }
        bool hasBinding(const QString& mrid) const { return idIndex.hasBinding(mrid); }
        bool hasSceneItem(const QString& mrid) const { return sceneItemForMrid(mrid) != nullptr; }
        QGraphicsItem* sceneItemForMrid(const QString& mrid) const { return shapeByMrid.value(mrid.trimmed(), nullptr); }
        QString graphicItemIdForMrid(const QString& mrid) const { return idIndex.graphicItemIdForMrid(mrid); }
        QString mridForGraphicItemId(const QString& graphicItemId) const { return idIndex.mridForGraphicItemId(graphicItemId); }
        QStringList mappedMrids() const { return idIndex.mrids(); }
        CimGraphicQueryContext queryContext() const { return {&shapeByMrid, &idIndex}; }
        CimGraphicQueryState graphicQueryState() const { return {queryContext(), generatedHelperLineCount}; }

        Summary summary() const
        {
            Summary result;
            result.sceneItemCount = sceneItemCount();
            result.bindingCount = bindingCount();
            result.generatedHelperLineCount = generatedHelperLineCount;
            return result;
        }
    };

    BuildResult populateSceneFromCimModel(CimdrawScene* scene, const CimModel& model) const;
};

#endif
