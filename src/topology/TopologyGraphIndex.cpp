#include "TopologyGraphIndex.h"

#include "Item/CimdrawConnectLine.h"
#include "Item/CimdrawGroup.h"
#include "Item/CimdrawItem.h"
#include "cim/ui/CimSceneBuilder.h"
#include "topology/TopologyBindingUtils.h"

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QSet>

namespace {

constexpr int kTopologyHlActive = static_cast<int>(Qt::UserRole) + 880;
constexpr int kTopologySavedPen = static_cast<int>(Qt::UserRole) + 881;
constexpr int kTopologySavedBrush = static_cast<int>(Qt::UserRole) + 882;
constexpr int kTopologySavedUsePen = static_cast<int>(Qt::UserRole) + 883;
constexpr int kTopologySavedUseBrush = static_cast<int>(Qt::UserRole) + 884;

void applyNodeHighlight(QGraphicsItem* item, bool on, const QColor& penC, const QColor& brushC)
{
    if (!item)
        return;
    auto applyLzItem = [&](CimdrawItem* li) {
        if (on) {
            if (li->data(kTopologyHlActive).toBool())
                return;
            li->setData(kTopologyHlActive, true);
            li->setData(kTopologySavedPen, li->getItemPenColor());
            li->setData(kTopologySavedBrush, li->getItemBrushColor());
            li->setData(kTopologySavedUsePen, li->getUsePen() ? 1 : 0);
            li->setData(kTopologySavedUseBrush, li->getUseBrush() ? 1 : 0);
            if (penC.isValid()) {
                li->setUsePen(true);
                li->setItemPenColor(penC);
            }
            if (brushC.isValid()) {
                li->setUseBrush(true);
                li->setItemBrushColor(brushC);
            }
            li->update();
        } else {
            if (!li->data(kTopologyHlActive).toBool())
                return;
            li->setData(kTopologyHlActive, false);
            if (li->data(kTopologySavedUsePen).isValid())
                li->setUsePen(li->data(kTopologySavedUsePen).toInt() != 0);
            if (li->data(kTopologySavedPen).canConvert<QColor>())
                li->setItemPenColor(li->data(kTopologySavedPen).value<QColor>());
            if (li->data(kTopologySavedUseBrush).isValid())
                li->setUseBrush(li->data(kTopologySavedUseBrush).toInt() != 0);
            if (li->data(kTopologySavedBrush).canConvert<QColor>())
                li->setItemBrushColor(li->data(kTopologySavedBrush).value<QColor>());
            li->update();
        }
    };

    if (auto* li = dynamic_cast<CimdrawItem*>(item))
        applyLzItem(li);
    else if (auto* grp = dynamic_cast<CimdrawGroup*>(item)) {
        // CimdrawGroup 无 CimdrawItem 基类，但 TmpBase 提供相同样式接口；此处用 QObject 动态属性不可行，直接调 Group 的 pen/brush
        if (on) {
            if (grp->data(kTopologyHlActive).toBool())
                return;
            grp->setData(kTopologyHlActive, true);
            grp->setData(kTopologySavedPen, grp->getItemPenColor());
            grp->setData(kTopologySavedBrush, grp->getItemBrushColor());
            grp->setData(kTopologySavedUsePen, grp->getUsePen() ? 1 : 0);
            grp->setData(kTopologySavedUseBrush, grp->getUseBrush() ? 1 : 0);
            if (penC.isValid()) {
                grp->setUsePen(true);
                grp->setItemPenColor(penC);
            }
            if (brushC.isValid()) {
                grp->setUseBrush(true);
                grp->setItemBrushColor(brushC);
            }
            grp->update();
        } else {
            if (!grp->data(kTopologyHlActive).toBool())
                return;
            grp->setData(kTopologyHlActive, false);
            if (grp->data(kTopologySavedUsePen).isValid())
                grp->setUsePen(grp->data(kTopologySavedUsePen).toInt() != 0);
            if (grp->data(kTopologySavedPen).canConvert<QColor>())
                grp->setItemPenColor(grp->data(kTopologySavedPen).value<QColor>());
            if (grp->data(kTopologySavedUseBrush).isValid())
                grp->setUseBrush(grp->data(kTopologySavedUseBrush).toInt() != 0);
            if (grp->data(kTopologySavedBrush).canConvert<QColor>())
                grp->setItemBrushColor(grp->data(kTopologySavedBrush).value<QColor>());
            grp->update();
        }
    }
}

} // namespace

void TopologyGraphIndex::clear()
{
    clearAllHighlights();
    m_nodeToEdgeIds.clear();
    m_edgeIdToMeta.clear();
    m_edgeIdToLine.clear();
    m_nodeIdToItem.clear();
}

void TopologyGraphIndex::rebuildFromScene(QGraphicsScene* scene)
{
    clear();
    if (!scene)
        return;

    const QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem* gi : items) {
        if (gi && gi->data(CimSceneBuilder::CimGeneratedVisualHelperDataKey).toBool())
            continue;
        if (dynamic_cast<CimdrawConnectLine*>(gi))
            continue;
        const QString nid = cimdrawTopologyBindingIdForItem(gi);
        if (!nid.isEmpty() && gi->parentItem() == nullptr)
            m_nodeIdToItem.insert(nid, gi);
    }

    for (QGraphicsItem* gi : items) {
        auto* line = dynamic_cast<CimdrawConnectLine*>(gi);
        if (!line)
            continue;
        if (line->data(CimSceneBuilder::CimGeneratedVisualHelperDataKey).toBool())
            continue;
        const TopologyEdgeMeta meta = line->resolvedRelationEdgeMeta();
        const QString eid = meta.edgeStableId;
        if (eid.isEmpty())
            continue;

        m_edgeIdToMeta.insert(eid, meta);
        m_edgeIdToLine.insert(eid, line);

        auto addEdgeToNode = [this](const QString& nodeId, const QString& edgeId) {
            if (nodeId.isEmpty())
                return;
            QStringList& lst = m_nodeToEdgeIds[nodeId];
            if (!lst.contains(edgeId))
                lst.append(edgeId);
        };
        addEdgeToNode(meta.startNodeStableId, eid);
        addEdgeToNode(meta.endNodeStableId, eid);
    }
}

QStringList TopologyGraphIndex::edgeIdsIncidentToNode(const QString& nodeStableId) const
{
    return m_nodeToEdgeIds.value(nodeStableId);
}

QStringList TopologyGraphIndex::adjacentNodeStableIds(const QString& nodeStableId) const
{
    QSet<QString> out;
    const QStringList edges = edgeIdsIncidentToNode(nodeStableId);
    for (const QString& eid : edges) {
        const TopologyEdgeMeta meta = m_edgeIdToMeta.value(eid);
        if (!meta.startNodeStableId.isEmpty() && meta.startNodeStableId != nodeStableId)
            out.insert(meta.startNodeStableId);
        if (!meta.endNodeStableId.isEmpty() && meta.endNodeStableId != nodeStableId)
            out.insert(meta.endNodeStableId);
    }
    QStringList list;
    list.reserve(out.size());
    for (const QString& s : out)
        list.append(s);
    return list;
}

bool TopologyGraphIndex::edgeMeta(const QString& edgeStableId, TopologyEdgeMeta* outMeta) const
{
    if (!outMeta || !m_edgeIdToMeta.contains(edgeStableId))
        return false;
    *outMeta = m_edgeIdToMeta.value(edgeStableId);
    return true;
}

QVector<TopologyEdgeMeta> TopologyGraphIndex::edgeMetas() const
{
    QVector<TopologyEdgeMeta> metas;
    metas.reserve(m_edgeIdToMeta.size());
    for (auto it = m_edgeIdToMeta.constBegin(); it != m_edgeIdToMeta.constEnd(); ++it)
        metas.push_back(it.value());
    return metas;
}

CimdrawConnectLine* TopologyGraphIndex::connectLineByEdgeId(const QString& edgeStableId) const
{
    return m_edgeIdToLine.value(edgeStableId).data();
}

QGraphicsItem* TopologyGraphIndex::nodeItemByStableId(const QString& nodeStableId) const
{
    return m_nodeIdToItem.value(nodeStableId, nullptr);
}

void TopologyGraphIndex::highlightEdges(const QStringList& edgeStableIds, bool highlighted, const QColor& strokeColor)
{
    const QColor c = strokeColor.isValid() ? strokeColor : QColor(255, 165, 0);
    for (const QString& eid : edgeStableIds) {
        CimdrawConnectLine* ln = connectLineByEdgeId(eid);
        if (!ln)
            continue;
        if (highlighted) {
            if (!ln->data(kTopologyHlActive).toBool()) {
                ln->setData(kTopologyHlActive, true);
                ln->setData(kTopologySavedPen, ln->getItemPenColor());
            }
            ln->setUsePen(true);
            ln->setItemPenColor(c);
            ln->update();
            if (!m_highlightedEdgeIds.contains(eid))
                m_highlightedEdgeIds.append(eid);
        } else {
            if (ln->data(kTopologyHlActive).toBool()) {
                ln->setData(kTopologyHlActive, false);
                if (ln->data(kTopologySavedPen).canConvert<QColor>())
                    ln->setItemPenColor(ln->data(kTopologySavedPen).value<QColor>());
            }
            m_highlightedEdgeIds.removeAll(eid);
            ln->update();
        }
    }
}

void TopologyGraphIndex::highlightNode(const QString& nodeStableId, bool highlighted, const QColor& penColor,
                                       const QColor& brushColor)
{
    QGraphicsItem* it = m_nodeIdToItem.value(nodeStableId, nullptr);
    if (!it)
        return;
    const QColor pen = penColor.isValid() ? penColor : QColor(255, 140, 0);
    const QColor brush = brushColor; // 可选
    applyNodeHighlight(it, highlighted, pen, brush);
    if (highlighted) {
        if (!m_highlightedNodeIds.contains(nodeStableId))
            m_highlightedNodeIds.append(nodeStableId);
    } else {
        m_highlightedNodeIds.removeAll(nodeStableId);
    }
}

void TopologyGraphIndex::clearAllHighlights()
{
    const QStringList edges = m_highlightedEdgeIds;
    highlightEdges(edges, false, QColor());
    m_highlightedEdgeIds.clear();

    const QStringList nodes = m_highlightedNodeIds;
    for (const QString& nid : nodes)
        highlightNode(nid, false, QColor(), QColor());
    m_highlightedNodeIds.clear();
}
