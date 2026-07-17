#include "CimdrawEditConnectLinePathCommand.h"

#include <QGraphicsItem>
#include <QLineF>
#include <QObject>

#include "item/CimdrawConnectLine.h"
#include "item/CimdrawConnectPoint.h"
#include "CimdrawView.h"

namespace {

constexpr qreal kPathEqualEps = 0.5;

bool scenePathsEqual(const QVector<QPointF>& a, const QVector<QPointF>& b)
{
    if (a.size() != b.size())
        return false;
    for (int i = 0; i < a.size(); ++i)
    {
        if (QLineF(a[i], b[i]).length() > kPathEqualEps)
            return false;
    }
    return true;
}

int connectPointOrdinalOnItem(QGraphicsItem* item, CimdrawConnectPoint* port)
{
    if (!item || !port)
        return -1;
    int ordinal = 0;
    for (QGraphicsItem* child : item->childItems())
    {
        if (auto* candidate = qgraphicsitem_cast<CimdrawConnectPoint*>(child))
        {
            if (candidate == port)
                return ordinal;
            ++ordinal;
        }
    }
    return -1;
}

CimdrawConnectPoint* connectPointByOrdinal(QGraphicsItem* item, int ordinal)
{
    if (!item || ordinal < 0)
        return nullptr;
    int current = 0;
    for (QGraphicsItem* child : item->childItems())
    {
        if (auto* port = qgraphicsitem_cast<CimdrawConnectPoint*>(child))
        {
            if (current == ordinal)
                return port;
            ++current;
        }
    }
    return nullptr;
}

bool statesEqual(const CimdrawEditConnectLinePathCommand::State& a,
                 const CimdrawEditConnectLinePathCommand::State& b)
{
    return scenePathsEqual(a.scenePoints, b.scenePoints)
        && a.routingMode == b.routingMode
        && a.startItemObject == b.startItemObject
        && a.endItemObject == b.endItemObject
        && a.startPortOrdinal == b.startPortOrdinal
        && a.endPortOrdinal == b.endPortOrdinal;
}

} // namespace

CimdrawEditConnectLinePathCommand::CimdrawEditConnectLinePathCommand(CimdrawConnectLine* line,
                                                           const State& beforeState,
                                                           const State& afterState,
                                                           QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_line(line)
    , m_beforeState(beforeState)
    , m_afterState(afterState)
{
    setText(QObject::tr("编辑连接线路径"));
}

CimdrawEditConnectLinePathCommand::State CimdrawEditConnectLinePathCommand::captureState(CimdrawConnectLine* line)
{
    State state;
    if (!line)
        return state;

    state.scenePoints = line->pathInSceneCoords();
    state.routingMode = line->pathRoutingMode();

    if (auto* startItem = line->getStartItem())
    {
        if (auto* obj = dynamic_cast<QObject*>(startItem))
            state.startItemObject = obj;
        state.startPortOrdinal = connectPointOrdinalOnItem(startItem, line->startConnectPort());
    }
    if (auto* endItem = line->getEndItem())
    {
        if (auto* obj = dynamic_cast<QObject*>(endItem))
            state.endItemObject = obj;
        state.endPortOrdinal = connectPointOrdinalOnItem(endItem, line->endConnectPort());
    }

    return state;
}

void CimdrawEditConnectLinePathCommand::applyState(const State& state)
{
    if (!m_line)
        return;

    auto* startItem = dynamic_cast<QGraphicsItem*>(state.startItemObject.data());
    auto* endItem = dynamic_cast<QGraphicsItem*>(state.endItemObject.data());

    m_line->cancelDeferredPathRecompute();
    m_line->setStartItem(startItem);
    m_line->setEndItem(endItem);
    m_line->setStartConnectPort(connectPointByOrdinal(startItem, state.startPortOrdinal));
    m_line->setEndConnectPort(connectPointByOrdinal(endItem, state.endPortOrdinal));
    // 撤销/重做必须按快照原样恢复；若再做端点纠正或 sanitize，
    // 端点附近刚被拖拽合并的局部轨迹会被二次改写。
    m_line->applyScenePathExact(state.scenePoints, state.routingMode);
    m_line->reattachToEndpointShapes(false);
    m_line->refreshResolvedTopologyBindings();
}

void CimdrawEditConnectLinePathCommand::undo()
{
    applyState(m_beforeState);
}

void CimdrawEditConnectLinePathCommand::redo()
{
    applyState(m_afterState);
}

void CimdrawEditConnectLinePathCommand::pushIfChanged(CimdrawView* view,
                                                 CimdrawConnectLine* line,
                                                 const State& beforeState,
                                                 const State& afterState,
                                                 const QString& actionText)
{
    if (!view || !line)
        return;
    if (statesEqual(beforeState, afterState))
        return;
    auto* cmd = new CimdrawEditConnectLinePathCommand(line, beforeState, afterState);
    if (!actionText.isEmpty())
        cmd->setText(actionText);
    view->getStack()->push(cmd);
}

int CimdrawEditConnectLinePathCommand::id() const
{
    return -1;
}

bool CimdrawEditConnectLinePathCommand::mergeWith(const QUndoCommand* other)
{
    Q_UNUSED(other);
    return false;
}
