#include "cim/query/CimGraphicSummaryText.h"

#include <QObject>
#include <QStringList>

namespace {

bool isSwitchLikeRole(PowerTopologyRole role)
{
    switch (role)
    {
    case PowerTopologyRole::Breaker:
    case PowerTopologyRole::Disconnector:
    case PowerTopologyRole::GroundSwitch:
        return true;
    case PowerTopologyRole::Busbar:
    case PowerTopologyRole::Ground:
    case PowerTopologyRole::Transformer:
    case PowerTopologyRole::Reactor:
    case PowerTopologyRole::Compensation:
    case PowerTopologyRole::SurgeArrester:
    case PowerTopologyRole::Measurement:
    case PowerTopologyRole::Feeder:
    case PowerTopologyRole::Conductor:
    case PowerTopologyRole::GenericEquipment:
    default:
        return false;
    }
}

void appendGraphicRenderStateDetailLines(QStringList* lines, const CimGraphicObjectSummary& objectSummary)
{
    if (!lines || !objectSummary.renderState.valid)
        return;

    lines->append(QObject::tr("显示运行态：%1")
                      .arg(cimGraphicRenderRunStateLabel(objectSummary.renderState.runState)));
    const QString switchLabel = cimGraphicRenderSwitchPositionLabel(objectSummary);
    if (!switchLabel.isEmpty())
        lines->append(QObject::tr("显示开关位置：%1").arg(switchLabel));
    const QString flowLabel = cimGraphicRenderFlowDirectionLabel(objectSummary);
    if (!flowLabel.isEmpty())
        lines->append(QObject::tr("显示流向符号：%1").arg(flowLabel));
    lines->append(QObject::tr("显示告警闪烁：%1")
                      .arg(cimGraphicRenderStateAlarmActive(objectSummary.renderState)
                               ? QObject::tr("是")
                               : QObject::tr("否")));
}

} // namespace

QString cimTopologyRelationTypeLabel(TopologyRelationType type)
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

QString cimGeneratedTopologyNodeLabel(const CimGeneratedTopologySourceSummary& summary)
{
    return summary.displayLabel.isEmpty() ? QObject::tr("未命名图元") : summary.displayLabel;
}

QString cimGraphicTopologyDomainLabel(CimdrawTopologyDomain domain)
{
    switch (domain)
    {
    case CimdrawTopologyDomain::PowerSystem:
        return QObject::tr("电力系统拓扑");
    case CimdrawTopologyDomain::None:
    default:
        return QObject::tr("关系拓扑");
    }
}

QString cimBehaviorTerminalConnectivityLabel(CimBehaviorTerminalConnectivity connectivity)
{
    switch (connectivity)
    {
    case CimBehaviorTerminalConnectivity::Disconnected:
        return QObject::tr("未连通");
    case CimBehaviorTerminalConnectivity::Partial:
        return QObject::tr("部分连通");
    case CimBehaviorTerminalConnectivity::Connected:
        return QObject::tr("已连通");
    case CimBehaviorTerminalConnectivity::Unknown:
    default:
        return QObject::tr("未知");
    }
}

QString cimGraphicObjectVisualStateLabel(CimGraphicObjectVisualState state)
{
    switch (state)
    {
    case CimGraphicObjectVisualState::Visualized:
        return QObject::tr("已可视化");
    case CimGraphicObjectVisualState::MappableNotInstantiated:
        return QObject::tr("应显示未实例化");
    case CimGraphicObjectVisualState::SemanticOnly:
        return QObject::tr("语义对象");
    case CimGraphicObjectVisualState::LayoutOnly:
        return QObject::tr("布局对象");
    case CimGraphicObjectVisualState::MetadataOnly:
        return QObject::tr("元数据/容器对象");
    case CimGraphicObjectVisualState::AbstractOnly:
        return QObject::tr("抽象层对象");
    case CimGraphicObjectVisualState::UnmappedDevice:
        return QObject::tr("设备未映射");
    case CimGraphicObjectVisualState::Uncategorized:
    default:
        return QObject::tr("未归类对象");
    }
}

QString cimGraphicPowerRoleLabel(PowerTopologyRole role)
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

QString cimBehaviorStateLabel(const CimBehaviorResult* behaviorResult)
{
    if (!behaviorResult || !behaviorResult->valid)
        return {};

    QStringList tags;
    if (behaviorResult->available)
        tags << QObject::tr("行为可用");
    if (behaviorResult->conductivity)
        tags << QObject::tr("导通");
    if (behaviorResult->energized)
        tags << QObject::tr("带电");
    if (behaviorResult->canOperate)
        tags << QObject::tr("可操作");
    return tags.join(QStringLiteral(" / "));
}

QString cimGraphicRenderRunStateLabel(CimdrawWiringRunState runState)
{
    switch (runState)
    {
    case CimdrawWiringRunState::Alarm:
        return QObject::tr("告警");
    case CimdrawWiringRunState::Energized:
        return QObject::tr("带电");
    case CimdrawWiringRunState::Deenergized:
        return QObject::tr("停运");
    case CimdrawWiringRunState::Normal:
    default:
        return QObject::tr("正常");
    }
}

QString cimGraphicRenderSwitchPositionLabel(const CimGraphicObjectSummary& objectSummary)
{
    if (!objectSummary.renderState.valid)
        return {};
    if (objectSummary.topologyDomain != CimdrawTopologyDomain::PowerSystem)
        return {};
    if (!isSwitchLikeRole(objectSummary.powerTopologyRole))
        return {};

    return cimGraphicRenderStateSwitchClosed(objectSummary.renderState)
        ? QObject::tr("合")
        : QObject::tr("分");
}

QString cimGraphicRenderFlowDirectionLabel(const CimGraphicObjectSummary& objectSummary)
{
    if (!objectSummary.renderState.valid || !objectSummary.supportsDirectedFlow)
        return {};

    return cimGraphicRenderStateFlowLeftToRight(objectSummary.renderState)
        ? QObject::tr("正向")
        : QObject::tr("反向");
}

QString cimGraphicRenderStateLabel(const CimGraphicObjectSummary& objectSummary)
{
    if (!objectSummary.renderState.valid)
        return {};

    QStringList tags;
    tags << QObject::tr("显示:%1").arg(cimGraphicRenderRunStateLabel(objectSummary.renderState.runState));

    const QString switchLabel = cimGraphicRenderSwitchPositionLabel(objectSummary);
    if (!switchLabel.isEmpty())
        tags << QObject::tr("开关:%1").arg(switchLabel);

    const QString flowLabel = cimGraphicRenderFlowDirectionLabel(objectSummary);
    if (!flowLabel.isEmpty())
        tags << QObject::tr("流向:%1").arg(flowLabel);

    if (cimGraphicRenderStateAlarmActive(objectSummary.renderState))
        tags << QObject::tr("告警闪烁");

    return tags.join(QStringLiteral(" / "));
}

QString cimGraphicObjectStatusLabel(const CimGraphicObjectSummary& objectSummary)
{
    QStringList sections;
    sections << cimGraphicObjectVisualStateLabel(objectSummary.visualState);

    const QString renderLabel = cimGraphicRenderStateLabel(objectSummary);
    if (!renderLabel.isEmpty())
        sections << renderLabel;

    const CimBehaviorResult* behaviorResult =
        objectSummary.behaviorResult.valid ? &objectSummary.behaviorResult : nullptr;
    const QString behaviorLabel = cimBehaviorStateLabel(behaviorResult);
    if (!behaviorLabel.isEmpty())
        sections << behaviorLabel;

    return sections.join(QStringLiteral(" | "));
}

QStringList cimRelationEdgeSummaryLines(const CimRelationEdgeSummary& relationSummary)
{
    const TopologyEdgeMeta& edgeInfo = relationSummary.edgeMeta;
    QStringList lines;
    lines << QObject::tr("对象类型：连接线");
    lines << QObject::tr("拓扑边 ID：%1").arg(edgeInfo.edgeStableId.isEmpty() ? QObject::tr("未生成")
                                                                            : edgeInfo.edgeStableId);
    lines << QObject::tr("关系类型：%1").arg(cimTopologyRelationTypeLabel(edgeInfo.relationType));
    lines << QObject::tr("起点节点 ID：%1").arg(edgeInfo.startNodeStableId.isEmpty() ? QObject::tr("空")
                                                                                     : edgeInfo.startNodeStableId);
    lines << QObject::tr("终点节点 ID：%1").arg(edgeInfo.endNodeStableId.isEmpty() ? QObject::tr("空")
                                                                                   : edgeInfo.endNodeStableId);
    lines << QObject::tr("起点端口：%1").arg(edgeInfo.startPortKey.isEmpty() ? QObject::tr("空")
                                                                             : edgeInfo.startPortKey);
    lines << QObject::tr("终点端口：%1").arg(edgeInfo.endPortKey.isEmpty() ? QObject::tr("空")
                                                                           : edgeInfo.endPortKey);
    lines << QObject::tr("是否语义拓扑关系线：%1").arg(relationSummary.relationLink
                                                            ? QObject::tr("是")
                                                            : QObject::tr("否"));
    return lines;
}

QStringList cimTopologyObjectSummaryLines(const CimGraphicObjectSummary& objectSummary,
                                          const CimTopologyObjectSummary& topologySummary)
{
    QStringList lines;
    const CimBehaviorResult& behaviorResult = objectSummary.behaviorResult;

    lines << QObject::tr("对象名称：%1").arg(objectSummary.displayName);
    lines << QObject::tr("类名：%1").arg(objectSummary.className.isEmpty() ? QObject::tr("未知")
                                                                          : objectSummary.className);
    lines << QObject::tr("对象 ID：%1").arg(objectSummary.objectId.isEmpty() ? QObject::tr("空")
                                                                            : objectSummary.objectId);
    lines << QObject::tr("mRID：%1").arg(objectSummary.mrid.isEmpty() ? QObject::tr("空")
                                                                       : objectSummary.mrid);
    lines << QObject::tr("可视化状态：%1").arg(cimGraphicObjectVisualStateLabel(objectSummary.visualState));
    lines << QObject::tr("拓扑节点 ID：%1").arg(topologySummary.nodeId.isEmpty() ? QObject::tr("空")
                                                                                : topologySummary.nodeId);
    lines << QObject::tr("是否参与拓扑：%1").arg(objectSummary.participatesInTopology ? QObject::tr("是")
                                                                                     : QObject::tr("否"));
    lines << QObject::tr("拓扑域：%1").arg(cimGraphicTopologyDomainLabel(objectSummary.topologyDomain));

    if (objectSummary.isTopologyGraphNode)
    {
        lines << QObject::tr("关系边数量：%1").arg(topologySummary.relationInfo.incidentEdgeCount());
        lines << QObject::tr("邻接节点数量：%1").arg(topologySummary.relationInfo.adjacentNodeCount());
    }

    if (objectSummary.topologyDomain == CimdrawTopologyDomain::PowerSystem)
    {
        lines << QObject::tr("电力角色：%1").arg(cimGraphicPowerRoleLabel(objectSummary.powerTopologyRole));
        appendGraphicRenderStateDetailLines(&lines, objectSummary);
        if (behaviorResult.valid)
        {
            lines << QObject::tr("行为可用：%1").arg(behaviorResult.available ? QObject::tr("是")
                                                                             : QObject::tr("否"));
            lines << QObject::tr("导通：%1").arg(behaviorResult.conductivity ? QObject::tr("是")
                                                                           : QObject::tr("否"));
            lines << QObject::tr("可操作：%1").arg(behaviorResult.canOperate ? QObject::tr("是")
                                                                           : QObject::tr("否"));
            lines << QObject::tr("带电：%1").arg(behaviorResult.energized ? QObject::tr("是")
                                                                         : QObject::tr("否"));
            lines << QObject::tr("端子连通性：%1").arg(
                cimBehaviorTerminalConnectivityLabel(behaviorResult.terminalConnectivity));
        }
        lines << QObject::tr("关联导体数：%1").arg(topologySummary.conductorIds.size());
        lines << QObject::tr("关联拓扑节点数：%1").arg(topologySummary.deviceNodeIds.size());
        lines << QObject::tr("当前连通设备数：%1").arg(topologySummary.connectedDevices.size());
        lines << QObject::tr("当前可达设备数：%1").arg(topologySummary.reachableDevices.size());
    }

    return lines;
}
