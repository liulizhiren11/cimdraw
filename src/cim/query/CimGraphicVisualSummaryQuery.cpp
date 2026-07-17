#include "cim/query/CimGraphicVisualSummaryQuery.h"

#include "cim/mapping/CimGraphicMapper.h"
#include "cim/mapping/CimIdIndex.h"
#include "cim/model/CimClassLineage.h"
#include "cim/model/CimModel.h"
#include "cim/query/CimGraphicObjectSummaryQuery.h"

namespace {

bool cimSemanticClass(const QString& className)
{
    return className == QStringLiteral("Terminal")
        || className == QStringLiteral("ACDCTerminal")
        || className == QStringLiteral("PowerTransformerEnd")
        || className == QStringLiteral("SvPowerFlow")
        || className == QStringLiteral("SvVoltage")
        || className == QStringLiteral("ConnectivityNode")
        || className == QStringLiteral("TopologicalNode");
}

bool cimLayoutClass(const QString& className)
{
    return className == QStringLiteral("Diagram")
        || className == QStringLiteral("DiagramObject")
        || className == QStringLiteral("DiagramObjectPoint")
        || className == QStringLiteral("CoordinateSystem");
}

bool cimMetadataContainerClass(const QString& className)
{
    return className == QStringLiteral("BaseVoltage")
        || className == QStringLiteral("Name")
        || className == QStringLiteral("NameType")
        || className == QStringLiteral("VoltageLevel")
        || className == QStringLiteral("Bay")
        || className == QStringLiteral("Substation")
        || className == QStringLiteral("SubGeographicalRegion")
        || className == QStringLiteral("GeographicalRegion")
        || className == QStringLiteral("OperationalLimit")
        || className == QStringLiteral("OperationalLimitSet")
        || className == QStringLiteral("OperationalLimitType")
        || className == QStringLiteral("VoltageLimit")
        || className == QStringLiteral("CurrentLimit");
}

bool cimAbstractCompatibilityClass(const QString& className)
{
    return className == QStringLiteral("IdentifiedObject")
        || className == QStringLiteral("Equipment")
        || className == QStringLiteral("ConductingEquipment")
        || className == QStringLiteral("Conductor")
        || className == QStringLiteral("PowerSystemResource")
        || className == QStringLiteral("ConnectivityNodeContainer")
        || className == QStringLiteral("Switch")
        || className == QStringLiteral("ProtectedSwitch")
        || className == QStringLiteral("RotatingMachine")
        || className == QStringLiteral("TransformerEnd")
        || className == QStringLiteral("EquipmentContainer");
}

bool looksLikeEquipmentClass(const QString& className)
{
    return CimClassLineage::isA(className, QStringLiteral("ConductingEquipment"))
        || CimClassLineage::isA(className, QStringLiteral("Equipment"));
}

} // namespace

CimGraphicVisualSummary CimGraphicVisualSummaryQuery::buildSummary(
    const CimModel& model,
    const QSet<QString>& visualizedMrids,
    int generatedHelperLineCount) const
{
    CimGraphicVisualSummary summary;
    summary.visualizedMrids = visualizedMrids;
    summary.generatedHelperLineCount = generatedHelperLineCount;

    CimGraphicMapper mapper;
    const QVector<CimObject> objects = model.allObjects();
    summary.totalObjects = objects.size();

    for (const CimObject& object : objects)
    {
        const CimGraphicMappingResult mapping = mapper.mapObject(object);
        const bool canMap = mapping.isMappable();
        const bool isVisualized =
            !object.mrid.trimmed().isEmpty() && summary.visualizedMrids.contains(object.mrid);
        CimGraphicObjectVisualState visualState = CimGraphicObjectVisualState::Uncategorized;
        if (canMap)
        {
            visualState = isVisualized
                ? CimGraphicObjectVisualState::Visualized
                : CimGraphicObjectVisualState::MappableNotInstantiated;
            ++summary.mappableObjects;
            if (isVisualized)
                ++summary.visualizedObjects;
            else
                ++summary.pendingObjects;
        }
        else if (cimSemanticClass(object.className))
        {
            visualState = CimGraphicObjectVisualState::SemanticOnly;
            ++summary.semanticObjects;
        }
        else if (cimLayoutClass(object.className))
        {
            visualState = CimGraphicObjectVisualState::LayoutOnly;
            ++summary.layoutObjects;
        }
        else if (cimMetadataContainerClass(object.className))
        {
            visualState = CimGraphicObjectVisualState::MetadataOnly;
            ++summary.metadataObjects;
        }
        else if (cimAbstractCompatibilityClass(object.className))
        {
            visualState = CimGraphicObjectVisualState::AbstractOnly;
            ++summary.abstractObjects;
        }
        else if (looksLikeEquipmentClass(object.className))
        {
            visualState = CimGraphicObjectVisualState::UnmappedDevice;
            ++summary.unmappedDeviceObjects;
            summary.unmappedDeviceClasses.insert(object.className);
        }
        else
        {
            visualState = CimGraphicObjectVisualState::Uncategorized;
            ++summary.unmappedObjects;
            summary.unmappedOtherClasses.insert(object.className);
        }

        if (!object.mrid.trimmed().isEmpty())
        {
            CimGraphicObjectSummary objectSummary;
            objectSummary.mrid = object.mrid;
            objectSummary.visualState = visualState;
            objectSummary.mappable = canMap;
            objectSummary.visualized = isVisualized;
            summary.objectSummariesByMrid.insert(object.mrid, objectSummary);
            summary.objectVisualStatesByMrid.insert(object.mrid, visualState);
        }

        if (object.className == QStringLiteral("ACLineSegment"))
        {
            ++summary.acLineSegmentObjects;
            if (isVisualized)
                ++summary.visualizedAcLineSegments;
        }
    }

    return summary;
}

CimGraphicVisualSummary CimGraphicVisualSummaryQuery::buildSummary(
    const CimModel& model,
    const CimGraphicQueryState& queryState) const
{
    CimGraphicVisualSummary summary =
        buildSummary(model, visualizedMrids(queryState), queryState.generatedHelperLineCount);
    const QHash<QString, QGraphicsItem*>* shapeByMrid = queryState.queryContext.shapeByMrid;
    if (!shapeByMrid)
        return summary;

    CimGraphicObjectSummaryQuery objectSummaryQuery;
    for (auto it = shapeByMrid->constBegin(); it != shapeByMrid->constEnd(); ++it)
    {
        if (it.key().trimmed().isEmpty() || !it.value())
            continue;

        auto objectSummaryIt = summary.objectSummariesByMrid.find(it.key());
        if (objectSummaryIt == summary.objectSummariesByMrid.end())
            continue;

        const CimGraphicObjectSummary liveSummary =
            objectSummaryQuery.objectSummaryForSceneItem(it.value(), queryState);
        objectSummaryIt->objectId = liveSummary.objectId;
        objectSummaryIt->displayName = liveSummary.displayName;
        objectSummaryIt->className = liveSummary.className;
        objectSummaryIt->participatesInTopology = liveSummary.participatesInTopology;
        objectSummaryIt->isTopologyGraphNode = liveSummary.isTopologyGraphNode;
        objectSummaryIt->supportsDirectedFlow = liveSummary.supportsDirectedFlow;
        objectSummaryIt->topologyDomain = liveSummary.topologyDomain;
        objectSummaryIt->powerTopologyRole = liveSummary.powerTopologyRole;
        objectSummaryIt->renderState = liveSummary.renderState;

        const CimBehaviorResult& behaviorResult = liveSummary.behaviorResult;
        if (!behaviorResult.valid)
            continue;

        objectSummaryIt->behaviorResult = behaviorResult;
        summary.behaviorResultsByMrid.insert(it.key(), behaviorResult);
        if (behaviorResult.available)
            ++summary.behaviorAvailableObjects;
        if (behaviorResult.conductivity)
            ++summary.conductiveObjects;
        if (behaviorResult.canOperate)
            ++summary.operableObjects;
        if (behaviorResult.energized)
            ++summary.energizedObjects;
    }

    return summary;
}

QSet<QString> CimGraphicVisualSummaryQuery::visualizedMrids(const CimGraphicQueryState& queryState) const
{
    return visualizedMrids(queryState.queryContext);
}

QSet<QString> CimGraphicVisualSummaryQuery::visualizedMrids(const CimGraphicQueryContext& queryContext) const
{
    const QHash<QString, QGraphicsItem*> emptyShapeByMrid;
    return visualizedMrids(queryContext.shapeByMrid ? *queryContext.shapeByMrid : emptyShapeByMrid,
                           queryContext.idIndex);
}

QSet<QString> CimGraphicVisualSummaryQuery::visualizedMrids(
    const QHash<QString, QGraphicsItem*>& shapeByMrid,
    const CimIdIndex* idIndex) const
{
    if (idIndex && idIndex->bindingCount() > 0)
    {
        QSet<QString> visualizedMrids;
        const QStringList mrids = idIndex->mrids();
        for (const QString& mrid : mrids)
            visualizedMrids.insert(mrid);
        return visualizedMrids;
    }

    QSet<QString> visualizedMrids;
    for (auto it = shapeByMrid.constBegin(); it != shapeByMrid.constEnd(); ++it)
    {
        if (it.value())
            visualizedMrids.insert(it.key());
    }
    return visualizedMrids;
}
