#ifndef TOPOLOGYGRAPHINDEX_H
#define TOPOLOGYGRAPHINDEX_H

#include "TopologyTypes.h"

#include <QColor>
#include <QHash>
#include <QPointer>
#include <QStringList>

class QGraphicsItem;
class QGraphicsScene;
class CimdrawConnectLine;

/**
 * @brief 场景级拓扑索引：节点–边邻接、边元数据、运行态高亮（仅 pen/brush，不改几何）
 */
class TopologyGraphIndex
{
public:
    TopologyGraphIndex() = default;

    void clear();
    void rebuildFromScene(QGraphicsScene* scene);

    QStringList edgeIdsIncidentToNode(const QString& nodeStableId) const;
    QStringList adjacentNodeStableIds(const QString& nodeStableId) const;
    bool edgeMeta(const QString& edgeStableId, TopologyEdgeMeta* outMeta) const;
    QVector<TopologyEdgeMeta> edgeMetas() const;
    CimdrawConnectLine* connectLineByEdgeId(const QString& edgeStableId) const;
    QGraphicsItem* nodeItemByStableId(const QString& nodeStableId) const;

    void highlightEdges(const QStringList& edgeStableIds, bool highlighted, const QColor& strokeColor = QColor());
    void highlightNode(const QString& nodeStableId, bool highlighted, const QColor& penColor = QColor(),
                       const QColor& brushColor = QColor());
    void clearAllHighlights();

private:
    QHash<QString, QStringList> m_nodeToEdgeIds;
    QHash<QString, TopologyEdgeMeta> m_edgeIdToMeta;
    QHash<QString, QPointer<CimdrawConnectLine>> m_edgeIdToLine;
    QHash<QString, QGraphicsItem*> m_nodeIdToItem;

    QStringList m_highlightedEdgeIds;
    QStringList m_highlightedNodeIds;
};

#endif // TOPOLOGYGRAPHINDEX_H
