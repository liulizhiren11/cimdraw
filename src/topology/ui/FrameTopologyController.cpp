#include "FrameTopologyController.h"

#include <QColor>
#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QHash>
#include <QMessageBox>
#include <QStringList>
#include <QtMath>

#include "Item/TmpBase.h"
#include "LzItemConfig.h"
#include "LzScene.h"
#include "LzView.h"
#include "item/LzConnectLine.h"
#include "item/LzTopologyNodeItem.h"
#include "topology/PowerTopologyBuilder.h"
#include "topology/TopologyGraphIndex.h"
#include "topology/TopologyTypes.h"
#include "wiring/base/LzWiringItemBase.h"

namespace {

QString topologyDomainLabel(LzTopologyDomain domain)
{
    switch (domain)
    {
    case LzTopologyDomain::PowerSystem:
        return QObject::tr("电力系统拓扑");
    case LzTopologyDomain::None:
    default:
        return QObject::tr("关系拓扑");
    }
}

QString topologyRelationTypeLabel(TopologyRelationType type)
{
    switch (type)
    {
    case TopologyRelationType::Physical:
        return QObject::tr("物理");
    case TopologyRelationType::Logical:
        return QObject::tr("逻辑");
    case TopologyRelationType::Flow:
        return QObject::tr("流向");
    case TopologyRelationType::Control:
        return QObject::tr("控制");
    case TopologyRelationType::Dependency:
        return QObject::tr("依赖");
    case TopologyRelationType::Reference:
        return QObject::tr("引用");
    }
    return QObject::tr("未知");
}

QString powerRoleLabel(PowerTopologyRole role)
{
    switch (role)
    {
    case PowerTopologyRole::Busbar:
        return QObject::tr("母线");
    case PowerTopologyRole::Breaker:
        return QObject::tr("断路器");
    case PowerTopologyRole::Disconnector:
        return QObject::tr("刀闸");
    case PowerTopologyRole::GroundSwitch:
        return QObject::tr("接地刀");
    case PowerTopologyRole::Ground:
        return QObject::tr("接地");
    case PowerTopologyRole::Transformer:
        return QObject::tr("变压器");
    case PowerTopologyRole::Reactor:
        return QObject::tr("电抗器");
    case PowerTopologyRole::Compensation:
        return QObject::tr("补偿设备");
    case PowerTopologyRole::SurgeArrester:
        return QObject::tr("避雷器");
    case PowerTopologyRole::Measurement:
        return QObject::tr("测量设备");
    case PowerTopologyRole::Feeder:
        return QObject::tr("馈线");
    case PowerTopologyRole::Conductor:
        return QObject::tr("导体");
    case PowerTopologyRole::GenericEquipment:
    default:
        return QObject::tr("通用设备");
    }
}

QString displayNameForShape(TmpShape* shape)
{
    if (!shape)
        return QString();
    const QString name = shape->displayName().trimmed();
    if (!name.isEmpty() && name != QStringLiteral("TmpBase"))
        return name;
    const QString cls = shape->className().trimmed();
    if (!cls.isEmpty())
        return cls;
    return QObject::tr("未命名图元");
}

QString topologyTabTitle(FrameTopologyController::GeneratedMode mode)
{
    switch (mode)
    {
    case FrameTopologyController::GeneratedMode::PowerSystem:
        return QObject::tr("电力拓扑图");
    case FrameTopologyController::GeneratedMode::Combined:
    default:
        return QObject::tr("关系拓扑图");
    }
}

bool matchesGeneratedTopologyMode(TmpShape* shape, FrameTopologyController::GeneratedMode mode)
{
    if (!shape)
        return false;
    if (!shape->participatesInTopology() && !shape->isTopologyGraphNode())
        return false;

    switch (mode)
    {
    case FrameTopologyController::GeneratedMode::PowerSystem:
        return shape->topologyDomain() == LzTopologyDomain::PowerSystem;
    case FrameTopologyController::GeneratedMode::Combined:
    default:
        return true;
    }
}

bool matchesGeneratedTopologyMode(const LzConnectLine* line,
                                  const QHash<QString, LzTopologyNodeItem*>& generatedNodes)
{
    if (!line)
        return false;
    return generatedNodes.contains(line->topologyStartNodeStableId())
        && generatedNodes.contains(line->topologyEndNodeStableId());
}

QString lineDisplayLabel(const LzConnectLine* line)
{
    if (!line)
        return QString();
    switch (line->topologyRelationType())
    {
    case TopologyRelationType::Flow:
        return QObject::tr("流向");
    case TopologyRelationType::Control:
        return QObject::tr("控制");
    case TopologyRelationType::Dependency:
        return QObject::tr("依赖");
    case TopologyRelationType::Logical:
        return QObject::tr("逻辑");
    case TopologyRelationType::Reference:
        return QObject::tr("引用");
    case TopologyRelationType::Physical:
    default:
        return QString();
    }
}

LzTopologyNodeItem* createTopologyNodeForShape(LzScene* targetScene,
                                               TmpShape* sourceShape,
                                               const QPointF& scenePos)
{
    if (!targetScene || !sourceShape)
        return nullptr;

    auto* node = new LzTopologyNodeItem(QRectF(0, 0, 96, 56));
    node->setPos(scenePos);
    node->setLzObjectId(sourceShape->topologyNodeStableId());
    targetScene->addItem(node);
    return node;
}

QGraphicsSimpleTextItem* createTopologyLabel(LzScene* targetScene, const QString& text, const QPointF& scenePos)
{
    if (!targetScene)
        return nullptr;
    auto* label = new QGraphicsSimpleTextItem(text);
    label->setPos(scenePos);
    targetScene->addItem(label);
    return label;
}

LzConnectLine::ArrowHeadStyle inferGeneratedTopologyArrow(const LzConnectLine* sourceLine)
{
    if (!sourceLine)
        return LzConnectLine::ArrowEnd;

    const LzConnectLine::ArrowHeadStyle sourceArrow = sourceLine->arrowHead();
    if (sourceArrow != LzConnectLine::ArrowNone)
        return sourceArrow;

    const auto* startWiring = dynamic_cast<LzWiringItemBase*>(sourceLine->getStartItem());
    const auto* endWiring = dynamic_cast<LzWiringItemBase*>(sourceLine->getEndItem());

    if (startWiring && startWiring->supportsFlowDirectionAction())
        return startWiring->flowSign() >= 0 ? LzConnectLine::ArrowEnd : LzConnectLine::ArrowStart;
    if (endWiring && endWiring->supportsFlowDirectionAction())
        return endWiring->flowSign() >= 0 ? LzConnectLine::ArrowStart : LzConnectLine::ArrowEnd;

    return LzConnectLine::ArrowEnd;
}

bool shouldReverseGeneratedTopologyDirection(const LzConnectLine* sourceLine)
{
    if (!sourceLine)
        return false;

    const auto* startWiring = dynamic_cast<LzWiringItemBase*>(sourceLine->getStartItem());
    const auto* endWiring = dynamic_cast<LzWiringItemBase*>(sourceLine->getEndItem());
    const PowerTopologyRole startRole = startWiring ? startWiring->powerTopologyRole() : PowerTopologyRole::GenericEquipment;
    const PowerTopologyRole endRole = endWiring ? endWiring->powerTopologyRole() : PowerTopologyRole::GenericEquipment;

    if (startRole != PowerTopologyRole::Busbar && endRole == PowerTopologyRole::Busbar)
        return true;
    if (startRole == PowerTopologyRole::Busbar && endRole != PowerTopologyRole::Busbar)
        return false;

    if (startWiring && startWiring->supportsFlowDirectionAction() && startWiring->flowSign() < 0)
        return true;
    if (endWiring && endWiring->supportsFlowDirectionAction() && endWiring->flowSign() > 0)
        return true;

    return false;
}

LzConnectPoint* preferredTopologyPort(TmpShape* shape, int preferredIndex)
{
    if (!shape)
        return nullptr;
    const QVector<LzConnectPoint*>& ports = shape->wiringConnectPorts();
    if (ports.isEmpty())
        return nullptr;
    if (preferredIndex >= 0 && preferredIndex < ports.size() && ports.at(preferredIndex))
        return ports.at(preferredIndex);
    for (LzConnectPoint* port : ports)
    {
        if (port)
            return port;
    }
    return nullptr;
}

LzConnectPoint* topologyPortForOutgoingNode(TmpShape* shape)
{
    return preferredTopologyPort(shape, RIGHT_DIRECTION);
}

LzConnectPoint* topologyPortForIncomingNode(TmpShape* shape)
{
    return preferredTopologyPort(shape, LEFT_DIRECTION);
}

QPointF topologyLabelPosition(const QPointF& nodePos)
{
    return nodePos + QPointF(-42.0, 44.0);
}

QPointF topologyRelationLabelPosition(LzConnectPoint* startPort, LzConnectPoint* endPort)
{
    if (!startPort || !endPort)
        return {};
    const QPointF startScene = startPort->connectionCenterInScene();
    const QPointF endScene = endPort->connectionCenterInScene();
    return (startScene + endScene) * 0.5 + QPointF(-16.0, -18.0);
}

bool buildGeneratedTopologyRelation(LzScene* targetScene,
                                    LzConnectLine* sourceLine,
                                    LzTopologyNodeItem* startNode,
                                    LzTopologyNodeItem* endNode)
{
    if (!targetScene || !sourceLine || !startNode || !endNode)
        return false;

    LzConnectPoint* startPort = topologyPortForOutgoingNode(startNode);
    LzConnectPoint* endPort = topologyPortForIncomingNode(endNode);
    if (!startPort || !endPort)
        return false;

    auto* topoLine = new LzConnectLine(QRectF(0, 0, 1, 1));
    targetScene->addItem(topoLine);
    topoLine->setStartItem(startNode);
    topoLine->setStartConnectPort(startPort);
    topoLine->setTopologyRelationType(sourceLine->topologyRelationType());
    topoLine->setArrowHead(inferGeneratedTopologyArrow(sourceLine));
    topoLine->attachEndAndRecompute(endNode, endPort);
    topoLine->ensureTopologyEdgeStableId();
    topoLine->syncTopologyEndpointsFromItems();
    topoLine->recomputePathFromCurrentEndpoints(false);

    const QString relationLabel = lineDisplayLabel(sourceLine);
    if (!relationLabel.isEmpty())
        createTopologyLabel(targetScene, relationLabel, topologyRelationLabelPosition(startPort, endPort));
    return true;
}

} // namespace

void FrameTopologyController::showSelectedSummary(QWidget* host, LzScene* scene) const
{
    if (!scene)
    {
        QMessageBox::information(host, QObject::tr("拓扑摘要"), QObject::tr("当前场景不可用。"));
        return;
    }

    const QList<QGraphicsItem*> selections = scene->getSelections();
    if (selections.count() != 1)
    {
        QMessageBox::information(host, QObject::tr("拓扑摘要"), QObject::tr("请先只选中一个图元。"));
        return;
    }

    QGraphicsItem* item = selections.first();
    QStringList lines;

    if (LzConnectLine* line = qgraphicsitem_cast<LzConnectLine*>(item))
    {
        lines << QObject::tr("对象类型：连接线");
        lines << QObject::tr("拓扑边 ID：%1").arg(line->topologyEdgeStableId().isEmpty()
                                                   ? QObject::tr("未生成")
                                                   : line->topologyEdgeStableId());
        lines << QObject::tr("关系类型：%1").arg(topologyRelationTypeLabel(line->topologyRelationType()));
        lines << QObject::tr("起点节点 ID：%1").arg(line->topologyStartNodeStableId().isEmpty()
                                                   ? QObject::tr("空")
                                                   : line->topologyStartNodeStableId());
        lines << QObject::tr("终点节点 ID：%1").arg(line->topologyEndNodeStableId().isEmpty()
                                                   ? QObject::tr("空")
                                                   : line->topologyEndNodeStableId());
        lines << QObject::tr("起点端口：%1").arg(line->topologyStartPortKey().isEmpty()
                                                 ? QObject::tr("空")
                                                 : line->topologyStartPortKey());
        lines << QObject::tr("终点端口：%1").arg(line->topologyEndPortKey().isEmpty()
                                                 ? QObject::tr("空")
                                                 : line->topologyEndPortKey());
        lines << QObject::tr("是否语义拓扑关系线：%1").arg(line->isTopologyRelationLink() ? QObject::tr("是")
                                                                                         : QObject::tr("否"));
        QMessageBox::information(host, QObject::tr("拓扑摘要"), lines.join('\n'));
        return;
    }

    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item);
    if (!shape)
    {
        QMessageBox::information(host, QObject::tr("拓扑摘要"), QObject::tr("当前选中对象不是可识别的拓扑图元。"));
        return;
    }

    lines << QObject::tr("对象名称：%1").arg(displayNameForShape(shape));
    lines << QObject::tr("类名：%1").arg(shape->className().isEmpty() ? QObject::tr("未知") : shape->className());
    lines << QObject::tr("对象 ID：%1").arg(shape->lzObjectId().isEmpty() ? QObject::tr("空") : shape->lzObjectId());
    lines << QObject::tr("拓扑节点 ID：%1").arg(shape->topologyNodeStableId().isEmpty() ? QObject::tr("空")
                                                                                       : shape->topologyNodeStableId());
    lines << QObject::tr("是否参与拓扑：%1").arg(shape->participatesInTopology() ? QObject::tr("是")
                                                                                : QObject::tr("否"));
    lines << QObject::tr("拓扑域：%1").arg(topologyDomainLabel(shape->topologyDomain()));

    if (shape->isTopologyGraphNode())
    {
        if (const TopologyGraphIndex* index = scene->topologyIndex())
        {
            const QString topologyId = shape->topologyNodeStableId();
            lines << QObject::tr("关系边数量：%1").arg(index->edgeIdsIncidentToNode(topologyId).size());
            lines << QObject::tr("邻接节点数量：%1").arg(index->adjacentNodeStableIds(topologyId).size());
        }
    }

    if (auto* wiring = dynamic_cast<LzWiringItemBase*>(shape))
    {
        if (shape->topologyDomain() == LzTopologyDomain::PowerSystem)
        {
            const QString deviceId = shape->topologyNodeStableId();
            lines << QObject::tr("电力角色：%1").arg(powerRoleLabel(wiring->powerTopologyRole()));
            lines << QObject::tr("关联导体数：%1").arg(scene->powerDeviceConductorIds(deviceId).size());
            lines << QObject::tr("关联拓扑节点数：%1").arg(scene->powerDeviceNodeIds(deviceId).size());
            lines << QObject::tr("当前连通设备数：%1").arg(scene->connectedPowerDevices(deviceId).size());
            lines << QObject::tr("当前可达设备数：%1").arg(scene->reachablePowerDevices(deviceId).size());
        }
    }

    QMessageBox::information(host, QObject::tr("拓扑摘要"), lines.join('\n'));
}

void FrameTopologyController::highlightSelected(QWidget* host, LzScene* scene) const
{
    if (!scene)
        return;

    const QList<QGraphicsItem*> selections = scene->getSelections();
    if (selections.count() != 1)
    {
        QMessageBox::information(host, QObject::tr("拓扑高亮"), QObject::tr("请先只选中一个图元。"));
        return;
    }

    scene->clearTopologyHighlights();

    QGraphicsItem* item = selections.first();
    if (LzConnectLine* line = qgraphicsitem_cast<LzConnectLine*>(item))
    {
        if (!line->topologyEdgeStableId().isEmpty())
            scene->highlightTopologyEdge(line->topologyEdgeStableId(), true, QColor(220, 80, 60));
        if (!line->topologyStartNodeStableId().isEmpty())
            scene->highlightTopologyNode(line->topologyStartNodeStableId(), true, QColor(255, 208, 0));
        if (!line->topologyEndNodeStableId().isEmpty())
            scene->highlightTopologyNode(line->topologyEndNodeStableId(), true, QColor(255, 208, 0));
        return;
    }

    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item);
    if (!shape)
        return;

    if (!shape->participatesInTopology())
    {
        QMessageBox::information(host, QObject::tr("拓扑高亮"), QObject::tr("当前选中图元未参与拓扑分析。"));
        return;
    }

    const QString topologyId = shape->topologyNodeStableId();
    scene->highlightTopologyNode(topologyId, true, QColor(255, 208, 0));

    if (shape->isTopologyGraphNode())
    {
        if (const TopologyGraphIndex* index = scene->topologyIndex())
        {
            const QStringList edgeIds = index->edgeIdsIncidentToNode(topologyId);
            scene->highlightTopologyEdges(edgeIds, true, QColor(220, 80, 60));
            const QStringList adjacentIds = index->adjacentNodeStableIds(topologyId);
            for (const QString& nodeId : adjacentIds)
                scene->highlightTopologyNode(nodeId, true, QColor(255, 208, 0));
        }
    }

    if (shape->topologyDomain() == LzTopologyDomain::PowerSystem)
    {
        const QStringList conductorIds = scene->powerDeviceConductorIds(topologyId);
        scene->highlightTopologyEdges(conductorIds, true, QColor(220, 80, 60));
        const QStringList devices = scene->connectedPowerDevices(topologyId);
        for (const QString& deviceId : devices)
            scene->highlightTopologyNode(deviceId, true, QColor(255, 208, 0));
    }
}

void FrameTopologyController::clearHighlights(LzScene* scene) const
{
    if (scene)
        scene->clearTopologyHighlights();
}

void FrameTopologyController::generateGraph(const GenerateContext& context, GeneratedMode mode) const
{
    if (!context.sourceScene)
    {
        QMessageBox::information(context.host, QObject::tr("生成拓扑图"), QObject::tr("当前场景不可用。"));
        return;
    }

    LzScene* sourceScene = context.sourceScene;
    sourceScene->rebuildTopologyIndex();

    if (mode == GeneratedMode::PowerSystem)
    {
        const PowerTopologyAnalysisSnapshot snapshot = PowerTopologyBuilder::build(sourceScene);
        if (snapshot.devices.isEmpty())
        {
            QMessageBox::information(context.host, QObject::tr("生成拓扑图"),
                                     QObject::tr("当前场景没有可生成的电力拓扑设备。"));
            return;
        }
    }

    QList<TmpShape*> sourceNodes;
    const QList<QGraphicsItem*> allItems = sourceScene->items();
    for (QGraphicsItem* item : allItems)
    {
        auto* shape = qgraphicsitem_cast<TmpShape*>(item);
        if (!shape || shape->parentItem())
            continue;
        if (!matchesGeneratedTopologyMode(shape, mode))
            continue;
        const QString topologyId = shape->ensureTopologyNodeStableId();
        if (topologyId.isEmpty())
            continue;
        sourceNodes.append(shape);
    }

    if (sourceNodes.isEmpty())
    {
        QMessageBox::information(context.host, QObject::tr("生成拓扑图"),
                                 QObject::tr("当前场景没有找到可参与拓扑的图元。"));
        return;
    }

    if (!context.createTargetView)
        return;

    LzView* targetView = context.createTargetView();
    if (!targetView)
        return;
    if (context.renameCurrentTab)
        context.renameCurrentTab(topologyTabTitle(mode));

    LzScene* targetScene = targetView->getScene();
    if (!targetScene)
        return;

    QHash<QString, LzTopologyNodeItem*> generatedNodes;
    const int count = sourceNodes.size();
    const int columns = qMax(1, qCeil(qSqrt(static_cast<double>(count))));
    const qreal xSpacing = 220.0;
    const qreal ySpacing = 160.0;
    const QPointF origin(120.0, 120.0);

    for (int i = 0; i < sourceNodes.size(); ++i)
    {
        TmpShape* shape = sourceNodes.at(i);
        const QString nodeId = shape->topologyNodeStableId();
        const int row = i / columns;
        const int col = i % columns;
        const QPointF pos(origin.x() + col * xSpacing, origin.y() + row * ySpacing);
        LzTopologyNodeItem* node = createTopologyNodeForShape(targetScene, shape, pos);
        if (!node)
            continue;
        generatedNodes.insert(nodeId, node);
        createTopologyLabel(targetScene, displayNameForShape(shape), topologyLabelPosition(pos));
    }

    int generatedLineCount = 0;
    for (QGraphicsItem* item : allItems)
    {
        auto* line = qgraphicsitem_cast<LzConnectLine*>(item);
        if (!line)
            continue;
        line->syncTopologyEndpointsFromItems();
        if (!matchesGeneratedTopologyMode(line, generatedNodes))
            continue;

        QString startId = line->topologyStartNodeStableId();
        QString endId = line->topologyEndNodeStableId();
        if (startId.isEmpty() || endId.isEmpty())
            continue;

        LzTopologyNodeItem* startNode = generatedNodes.value(startId);
        LzTopologyNodeItem* endNode = generatedNodes.value(endId);
        if (!startNode || !endNode)
            continue;

        if (shouldReverseGeneratedTopologyDirection(line))
        {
            qSwap(startNode, endNode);
            qSwap(startId, endId);
        }

        if (buildGeneratedTopologyRelation(targetScene, line, startNode, endNode))
            ++generatedLineCount;
    }

    targetScene->rebuildTopologyIndex();
    targetView->setModified(true);
    if (context.onGenerated)
        context.onGenerated();

    if (generatedLineCount <= 0)
    {
        QMessageBox::information(context.host,
                                 QObject::tr("生成拓扑图"),
                                 QObject::tr("已生成拓扑节点，但当前没有找到可连接的拓扑关系线。"));
    }
}
