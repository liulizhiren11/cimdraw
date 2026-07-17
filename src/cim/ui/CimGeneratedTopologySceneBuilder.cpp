#include "cim/ui/CimGeneratedTopologySceneBuilder.h"

#include <QGraphicsSimpleTextItem>
#include <QHash>
#include <QStringList>

#include "CimdrawScene.h"
#include "Item/TmpBase.h"
#include "cim/query/CimGeneratedTopologyLayout.h"
#include "cim/query/CimGraphicSummaryText.h"
#include "item/CimdrawConnectLine.h"
#include "item/CimdrawTopologyNodeItem.h"
#include "topology/TopologyTypes.h"

namespace {

bool matchesGeneratedTopologyMode(const TopologyEdgeMeta& edgeMeta,
                                  const QHash<QString, CimdrawTopologyNodeItem*>& generatedNodes)
{
    return generatedNodes.contains(edgeMeta.startNodeStableId)
        && generatedNodes.contains(edgeMeta.endNodeStableId);
}

CimdrawTopologyNodeItem* createTopologyNodeForShape(CimdrawScene* targetScene,
                                                    const QString& sourceNodeId,
                                                    const QPointF& scenePos)
{
    if (!targetScene || sourceNodeId.isEmpty())
        return nullptr;

    auto* node = new CimdrawTopologyNodeItem(QRectF(0, 0, 96, 56));
    node->setPos(scenePos);
    node->setCimdrawObjectId(sourceNodeId);
    targetScene->addItem(node);
    return node;
}

QGraphicsSimpleTextItem* createTopologyLabel(CimdrawScene* targetScene,
                                             const QString& text,
                                             const QPointF& scenePos)
{
    if (!targetScene)
        return nullptr;
    auto* label = new QGraphicsSimpleTextItem(text);
    label->setPos(scenePos);
    targetScene->addItem(label);
    return label;
}

CimdrawConnectPoint* preferredTopologyPort(TmpShape* shape, int preferredIndex)
{
    if (!shape)
        return nullptr;
    const QVector<CimdrawConnectPoint*>& ports = shape->wiringConnectPorts();
    if (ports.isEmpty())
        return nullptr;
    if (preferredIndex >= 0 && preferredIndex < ports.size() && ports.at(preferredIndex))
        return ports.at(preferredIndex);
    for (CimdrawConnectPoint* port : ports)
    {
        if (port)
            return port;
    }
    return nullptr;
}

CimdrawConnectPoint* topologyPortByDirection(TmpShape* shape, CONNECT_DIRECTION direction)
{
    if (!shape)
        return nullptr;
    const QVector<CimdrawConnectPoint*>& ports = shape->wiringConnectPorts();
    for (CimdrawConnectPoint* port : ports)
    {
        if (port && port->getDirection() == direction)
            return port;
    }
    return nullptr;
}

CimdrawConnectPoint* topologyPortByKey(TmpShape* shape, const QString& portKey)
{
    if (!shape)
        return nullptr;

    bool ok = false;
    const int preferredIndex = portKey.trimmed().toInt(&ok);
    if (ok)
        return preferredTopologyPort(shape, preferredIndex);

    const QString trimmedKey = portKey.trimmed();
    if (trimmedKey.startsWith(QStringLiteral("dir:")))
    {
        const QStringList parts = trimmedKey.split(QLatin1Char(':'));
        if (parts.size() >= 2)
        {
            const int directionValue = parts.at(1).toInt(&ok);
            if (ok)
            {
                if (CimdrawConnectPoint* port =
                        topologyPortByDirection(shape, static_cast<CONNECT_DIRECTION>(directionValue)))
                {
                    return port;
                }
            }
        }
    }

    const QVector<CimdrawConnectPoint*>& ports = shape->wiringConnectPorts();
    for (CimdrawConnectPoint* port : ports)
    {
        if (port && port->portTag().trimmed() == trimmedKey)
            return port;
    }

    return nullptr;
}

CimdrawConnectPoint* topologyPortForOutgoingNode(TmpShape* shape)
{
    return preferredTopologyPort(shape, RIGHT_DIRECTION);
}

CimdrawConnectPoint* topologyPortForIncomingNode(TmpShape* shape)
{
    return preferredTopologyPort(shape, LEFT_DIRECTION);
}

CimdrawConnectPoint* topologyPortForRelationEndpoint(TmpShape* shape,
                                                     const QString& portKey,
                                                     bool outgoing)
{
    if (CimdrawConnectPoint* port = topologyPortByKey(shape, portKey))
        return port;
    return outgoing ? topologyPortForOutgoingNode(shape) : topologyPortForIncomingNode(shape);
}

bool buildGeneratedTopologyRelation(CimdrawScene* targetScene,
                                    const CimGeneratedTopologyRelationSummary& relationSummary,
                                    CimdrawTopologyNodeItem* startNode,
                                    CimdrawTopologyNodeItem* endNode)
{
    if (!targetScene || !startNode || !endNode)
        return false;

    CimdrawConnectPoint* startPort =
        topologyPortForRelationEndpoint(startNode, relationSummary.startPortKey, true);
    CimdrawConnectPoint* endPort =
        topologyPortForRelationEndpoint(endNode, relationSummary.endPortKey, false);
    if (!startPort || !endPort)
        return false;

    auto* topoLine = new CimdrawConnectLine(QRectF(0, 0, 1, 1));
    targetScene->addItem(topoLine);
    topoLine->setStartItem(startNode);
    topoLine->setStartConnectPort(startPort);
    topoLine->setArrowHead(
        static_cast<CimdrawConnectLine::ArrowHeadStyle>(relationSummary.arrowHeadStyle));
    topoLine->applyRelationEdgeMeta(TopologyEdgeMeta{relationSummary.edgeStableId,
                                                     relationSummary.relationType,
                                                     relationSummary.startNodeId,
                                                     relationSummary.endNodeId,
                                                     relationSummary.startPortKey,
                                                     relationSummary.endPortKey});
    topoLine->attachEndAndRecompute(endNode, endPort);
    topoLine->refreshResolvedTopologyBindings();
    topoLine->recomputePathFromCurrentEndpoints(false);

    const QString relationLabel = relationSummary.relationLabel;
    if (!relationLabel.isEmpty())
    {
        createTopologyLabel(targetScene,
                            relationLabel,
                            cimGeneratedTopologyRelationLabelPosition(startPort->connectionCenterInScene(),
                                                                      endPort->connectionCenterInScene(),
                                                                      relationSummary));
    }
    return true;
}

} // namespace

CimGeneratedTopologySceneBuildResult cimRenderGeneratedTopologyGraphSummary(
    CimdrawScene* targetScene,
    const CimGeneratedTopologyGraphSummary& graphSummary)
{
    CimGeneratedTopologySceneBuildResult result;
    if (!targetScene || graphSummary.isEmpty())
        return result;

    QHash<QString, CimdrawTopologyNodeItem*> generatedNodes;
    for (int i = 0; i < graphSummary.nodeSummaries.size(); ++i)
    {
        const CimGeneratedTopologySourceSummary& sourceSummary = graphSummary.nodeSummaries.at(i);
        const QString& nodeId = sourceSummary.nodeId;
        const QPointF pos = graphSummary.nodeScenePos(i);
        CimdrawTopologyNodeItem* node = createTopologyNodeForShape(targetScene, nodeId, pos);
        if (!node)
            continue;
        generatedNodes.insert(nodeId, node);
        ++result.generatedNodeCount;
        createTopologyLabel(targetScene,
                            cimGeneratedTopologyNodeLabel(sourceSummary),
                            cimGeneratedTopologyNodeLabelPosition(pos, sourceSummary));
    }

    for (const CimGeneratedTopologyRelationSummary& relationSummary : graphSummary.relationSummaries)
    {
        if (!relationSummary.isValid())
            continue;
        TopologyEdgeMeta edgeMeta;
        edgeMeta.edgeStableId = relationSummary.edgeStableId;
        edgeMeta.relationType = relationSummary.relationType;
        edgeMeta.startNodeStableId = relationSummary.startNodeId;
        edgeMeta.endNodeStableId = relationSummary.endNodeId;
        if (!matchesGeneratedTopologyMode(edgeMeta, generatedNodes))
            continue;

        CimdrawTopologyNodeItem* startNode = generatedNodes.value(relationSummary.startNodeId);
        CimdrawTopologyNodeItem* endNode = generatedNodes.value(relationSummary.endNodeId);
        if (!startNode || !endNode)
            continue;

        if (buildGeneratedTopologyRelation(targetScene, relationSummary, startNode, endNode))
            ++result.generatedLineCount;
    }

    return result;
}
