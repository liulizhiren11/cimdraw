#ifndef CIMDRAWSCENETOPOLOGYSTATECONTROLLER_H
#define CIMDRAWSCENETOPOLOGYSTATECONTROLLER_H

#include <memory>
#include <QStringList>

#include "topology/PowerTopologyTypes.h"

class QColor;
class QString;
class CimdrawScene;
class TopologyGraphIndex;

class CimdrawSceneTopologyStateController
{
public:
    CimdrawSceneTopologyStateController();
    ~CimdrawSceneTopologyStateController();
    CimdrawSceneTopologyStateController(const CimdrawSceneTopologyStateController&) = delete;
    CimdrawSceneTopologyStateController& operator=(const CimdrawSceneTopologyStateController&) = delete;
    CimdrawSceneTopologyStateController(CimdrawSceneTopologyStateController&&) noexcept;
    CimdrawSceneTopologyStateController& operator=(CimdrawSceneTopologyStateController&&) noexcept;

    void invalidateSnapshots();

    void rebuildTopologyIndex(CimdrawScene* scene);
    TopologyNodeRelationInfo relationNodeInfo(CimdrawScene* scene, const QString& nodeStableId);
    TopologyEdgeMeta relationEdgeMeta(CimdrawScene* scene, const QString& edgeStableId);
    TopologyEdgeMeta resolveRelationEdgeMeta(CimdrawScene* scene, const TopologyEdgeLookupHint& lookupHint);
    QVector<TopologyEdgeMeta> relationEdgeMetas(CimdrawScene* scene);

    void highlightTopologyNode(const QString& nodeStableId, bool on, const QColor& c);
    void highlightTopologyEdge(const QString& edgeStableId, bool on, const QColor& c);
    void highlightTopologyEdges(const QStringList& edgeStableIds, bool on, const QColor& c);
    void clearTopologyHighlights();

    PowerTopologyAnalysisSnapshot buildPowerTopologySnapshot(CimdrawScene* scene);
    PowerTopologyDocumentExport buildPowerTopologyDocumentExport(CimdrawScene* scene);

private:
    const TopologyGraphIndex* topologyIndex() const;
    TopologyGraphIndex* topologyIndex();
    bool powerTopologySnapshotDirty_ = true;
    std::unique_ptr<PowerTopologyAnalysisSnapshot> powerTopologySnapshot_;
    std::unique_ptr<TopologyGraphIndex> topologyIndex_;
};

#endif
