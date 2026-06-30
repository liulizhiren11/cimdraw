#ifndef LZSCENETOPOLOGYSTATECONTROLLER_H
#define LZSCENETOPOLOGYSTATECONTROLLER_H

#include <memory>
#include <QStringList>

#include "topology/PowerTopologyTypes.h"

class QColor;
class QString;
class LzScene;
class TopologyGraphIndex;

class LzSceneTopologyStateController
{
public:
    LzSceneTopologyStateController();
    ~LzSceneTopologyStateController();
    LzSceneTopologyStateController(const LzSceneTopologyStateController&) = delete;
    LzSceneTopologyStateController& operator=(const LzSceneTopologyStateController&) = delete;
    LzSceneTopologyStateController(LzSceneTopologyStateController&&) noexcept;
    LzSceneTopologyStateController& operator=(LzSceneTopologyStateController&&) noexcept;

    void invalidateSnapshots();

    void rebuildTopologyIndex(LzScene* scene);
    const TopologyGraphIndex* topologyIndex() const;
    TopologyGraphIndex* topologyIndex();

    void highlightTopologyNode(const QString& nodeStableId, bool on, const QColor& c);
    void highlightTopologyEdge(const QString& edgeStableId, bool on, const QColor& c);
    void highlightTopologyEdges(const QStringList& edgeStableIds, bool on, const QColor& c);
    void clearTopologyHighlights();

    PowerTopologyAnalysisSnapshot buildPowerTopologySnapshot(LzScene* scene);
    PowerTopologyDocumentExport buildPowerTopologyDocumentExport(LzScene* scene);

private:
    bool powerTopologySnapshotDirty_ = true;
    std::unique_ptr<PowerTopologyAnalysisSnapshot> powerTopologySnapshot_;
    std::unique_ptr<TopologyGraphIndex> topologyIndex_;
};

#endif
