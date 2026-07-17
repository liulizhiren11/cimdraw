#include "CimdrawMimeData.h"
#include "item/TmpBase.h"
#include "item/CimdrawConnectLine.h"
#include "item/CimdrawGroup.h"
#include "item/CimdrawItem.h"
#include "CimdrawConnectConfig.h"
#include "topology/TopologyBindingUtils.h"

#include <QGraphicsScene>
#include <QHash>
#include <QPair>
#include <QSet>

namespace {

bool isClipboardHandle(const QGraphicsItem* item)
{
    return item && item->type() == CimdrawHandle::Type;
}

CimdrawItem* asStandaloneShape(QGraphicsItem* item)
{
    return dynamic_cast<CimdrawItem*>(item);
}

CimdrawGroup* asGroupItem(QGraphicsItem* item)
{
    return dynamic_cast<CimdrawGroup*>(item);
}

QGraphicsItem* duplicateClipboardItem(QGraphicsItem* item)
{
    if (auto* grp = asGroupItem(item))
        return grp->duplicate();
    if (auto* shape = asStandaloneShape(item))
        return shape->duplicate();
    return nullptr;
}

QString stableNodeIdForPaste(QGraphicsItem* it)
{
    if (!it)
        return {};
    if (auto* shape = asStandaloneShape(it))
        return shape->ensureCimdrawObjectId();
    return {};
}

void collectShapeLeaves(QGraphicsItem* root, QSet<QGraphicsItem*>& shapes)
{
    if (!root || isClipboardHandle(root))
        return;
    if (auto* grp = dynamic_cast<CimdrawGroup*>(root))
    {
        for (QGraphicsItem* child : grp->childItems())
            collectShapeLeaves(child, shapes);
        return;
    }
    if (qgraphicsitem_cast<CimdrawConnectLine*>(root))
        return;
    if (asStandaloneShape(root))
        shapes.insert(root);
}

bool isDescendantOfSelectedGroup(QGraphicsItem* item, const QList<QGraphicsItem*>& tops)
{
    for (QGraphicsItem* p = item ? item->parentItem() : nullptr; p; p = p->parentItem())
    {
        if (tops.contains(p))
            return true;
    }
    return false;
}

void collectConnectLinesInsideGroups(const QList<QGraphicsItem*>& topGroups,
                                   QSet<QGraphicsItem*>& out)
{
    for (QGraphicsItem* top : topGroups)
    {
        auto* grp = dynamic_cast<CimdrawGroup*>(top);
        if (!grp)
            continue;
        for (QGraphicsItem* child : grp->childItems())
        {
            if (auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(child))
                out.insert(line);
        }
    }
}

QList<QGraphicsItem*> expandClipboardItems(const QList<QGraphicsItem*>& items)
{
    QList<QGraphicsItem*> result;
    QSet<QGraphicsItem*> shapeScope;
    QList<QGraphicsItem*> topGroups;
    QSet<QGraphicsItem*> linesInsideSelectedGroups;

    for (QGraphicsItem* item : items)
    {
        if (!item || isClipboardHandle(item))
            continue;
        if (auto* grp = dynamic_cast<CimdrawGroup*>(item))
        {
            if (!result.contains(grp))
                result.append(grp);
            topGroups.append(grp);
            collectShapeLeaves(grp, shapeScope);
            collectConnectLinesInsideGroups({grp}, linesInsideSelectedGroups);
            continue;
        }
        if (auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item))
        {
            if (!isDescendantOfSelectedGroup(line, topGroups) && !result.contains(line))
                result.append(line);
            continue;
        }
        if (asStandaloneShape(item) && !isDescendantOfSelectedGroup(item, topGroups))
        {
            shapeScope.insert(item);
            if (!result.contains(item))
                result.append(item);
        }
    }

    QGraphicsScene* scene = nullptr;
    for (QGraphicsItem* item : items)
    {
        if (item && item->scene())
        {
            scene = item->scene();
            break;
        }
    }
    // 分组通常只包含图元本身，内部连线仍可能留在场景顶层，因此这里统一按端点归属补齐一次。
    // 若连线本身已经挂在被选分组下，isDescendantOfSelectedGroup/result.contains 会避免重复纳入。
    if (scene && !shapeScope.isEmpty())
    {
        for (QGraphicsItem* si : scene->items())
        {
            auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(si);
            if (!line)
                continue;
            QGraphicsItem* start = line->getStartItem();
            QGraphicsItem* end = line->getEndItem();
            if (linesInsideSelectedGroups.contains(line))
                continue;
            if (isDescendantOfSelectedGroup(line, topGroups))
                continue;
            if (shapeScope.contains(start) && shapeScope.contains(end) && !result.contains(line))
                result.append(line);
        }
    }

    return result;
}

void collectGroupChildrenInOrder(const CimdrawGroup* group, QList<QGraphicsItem*>& out)
{
    out.clear();
    if (!group)
        return;
    for (QGraphicsItem* c : group->childItems())
    {
        if (!isClipboardHandle(c))
            out.append(c);
    }
}

void collectGroupChildLinesInOrder(const CimdrawGroup* group, QList<CimdrawConnectLine*>& out)
{
    out.clear();
    if (!group)
        return;
    for (QGraphicsItem* child : group->childItems())
    {
        if (isClipboardHandle(child))
            continue;
        if (auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(child))
            out.append(line);
    }
}

QString stableEndpointIdForPaste(CimdrawConnectLine* line, bool startEndpoint)
{
    if (!line)
        return {};
    QString id = stableNodeIdForPaste(startEndpoint ? line->getStartItem() : line->getEndItem());
    if (!id.isEmpty())
        return id;
    return line->persistedEndpointBindingId(startEndpoint);
}

void mapGroupConnectLines(CimdrawGroup* tplGrp,
                          CimdrawGroup* freshGrp,
                          QHash<QGraphicsItem*, QGraphicsItem*>& outTplLineToFreshLine)
{
    outTplLineToFreshLine.clear();
    if (!tplGrp || !freshGrp)
        return;
    QList<CimdrawConnectLine*> tplLines;
    QList<CimdrawConnectLine*> freshLines;
    collectGroupChildLinesInOrder(tplGrp, tplLines);
    collectGroupChildLinesInOrder(freshGrp, freshLines);
    const int n = qMin(tplLines.size(), freshLines.size());
    for (int i = 0; i < n; ++i)
        outTplLineToFreshLine.insert(tplLines.at(i), freshLines.at(i));
}

void registerGroupClipboardLineEndpoints(CimdrawGroup* origGrp,
                                       CimdrawGroup* tplGrp,
                                       QHash<QGraphicsItem*, QPair<QString, QString>>& lineTemplateToOrigEndpointIds,
                                       QHash<QGraphicsItem*, QVector<QPointF>>& lineTemplateScenePaths,
                                       QHash<QGraphicsItem*, ConnectorPathRoutingMode>& lineTemplateRoutingModes)
{
    if (!origGrp || !tplGrp)
        return;

    QList<CimdrawConnectLine*> origLines;
    QList<CimdrawConnectLine*> tplLines;
    collectGroupChildLinesInOrder(origGrp, origLines);
    collectGroupChildLinesInOrder(tplGrp, tplLines);

    const int n = qMin(origLines.size(), tplLines.size());
    for (int i = 0; i < n; ++i)
    {
        auto* oLine = origLines.at(i);
        auto* tLine = tplLines.at(i);
        const QString a = stableEndpointIdForPaste(oLine, true);
        const QString b = stableEndpointIdForPaste(oLine, false);
        if (a.isEmpty() || b.isEmpty())
            continue;
        const QVector<QPointF> scenePath = oLine->pathInSceneCoords();
        if (scenePath.size() < 2)
            continue;
        lineTemplateToOrigEndpointIds.insert(tLine, qMakePair(a, b));
        lineTemplateScenePaths.insert(tLine, scenePath);
        lineTemplateRoutingModes.insert(tLine, oLine->pathRoutingMode());
    }
}

bool rewireConnectLinePair(CimdrawConnectLine* tplLine,
                           CimdrawConnectLine* freshLine,
                           const QPointF& deltaScene,
                           const QHash<QString, QGraphicsItem*>& origObjectIdToFresh,
                           const QHash<QString, QGraphicsItem*>& topologyBindingIdToFresh,
                           const QHash<QGraphicsItem*, QString>& nodeTemplateToOrigStableId,
                           const QHash<QGraphicsItem*, QPair<QString, QString>>& lineTemplateToOrigEndpointIds,
                           const QHash<QGraphicsItem*, QVector<QPointF>>& lineTemplateScenePaths,
                           const QHash<QGraphicsItem*, ConnectorPathRoutingMode>& lineTemplateRoutingModes)
{
    if (!tplLine || !freshLine)
        return false;

    QString startId;
    QString endId;
    if (const auto ends = lineTemplateToOrigEndpointIds.constFind(tplLine);
        ends != lineTemplateToOrigEndpointIds.constEnd())
    {
        startId = ends.value().first;
        endId = ends.value().second;
    }
    else
    {
        startId = nodeTemplateToOrigStableId.value(tplLine->getStartItem());
        endId = nodeTemplateToOrigStableId.value(tplLine->getEndItem());
    }
    if (startId.isEmpty())
        startId = tplLine->persistedEndpointBindingId(true);
    if (endId.isEmpty())
        endId = tplLine->persistedEndpointBindingId(false);
    if (startId.isEmpty() || endId.isEmpty())
        return false;

    QGraphicsItem* startFresh = origObjectIdToFresh.value(startId);
    QGraphicsItem* endFresh = origObjectIdToFresh.value(endId);
    if (!startFresh)
        startFresh = topologyBindingIdToFresh.value(tplLine->persistedEndpointBindingId(true));
    if (!endFresh)
        endFresh = topologyBindingIdToFresh.value(tplLine->persistedEndpointBindingId(false));
    if (!startFresh || !endFresh)
    {
        qWarning().noquote()
            << "rewireConnectLinePair failed:"
            << "startId=" << startId
            << "endId=" << endId
            << "tplStartTopo=" << tplLine->persistedEndpointBindingId(true)
            << "tplEndTopo=" << tplLine->persistedEndpointBindingId(false)
            << "hasStartFresh=" << (startFresh != nullptr)
            << "hasEndFresh=" << (endFresh != nullptr)
            << "objectIdKeys=" << origObjectIdToFresh.keys()
            << "topologyBindingKeys=" << topologyBindingIdToFresh.keys();
        return false;
    }

    freshLine->setStartItem(startFresh);
    freshLine->setEndItem(endFresh);

    QVector<QPointF> scenePath = lineTemplateScenePaths.value(tplLine);
    if (scenePath.size() < 2)
        scenePath = tplLine->pathInSceneCoords();
    for (QPointF& p : scenePath)
        p += deltaScene;
    if (scenePath.size() < 2)
    {
        const QPointF startScene = startFresh->mapToScene(startFresh->boundingRect().center());
        const QPointF endScene = endFresh->mapToScene(endFresh->boundingRect().center());
        scenePath = {startScene, endScene};
    }
    const ConnectorPathRoutingMode routingMode =
        lineTemplateRoutingModes.value(tplLine, tplLine->pathRoutingMode());

    freshLine->restorePersistedEndpointPorts(scenePath);
    freshLine->applyScenePathExact(scenePath, routingMode);
    freshLine->snapAttachedBusbarEndpointsToPorts();
    freshLine->registerEndpointAttachments();
    freshLine->cancelDeferredPathRecompute();
    return true;
}

void registerNodeTemplateIds(QGraphicsItem* orig, QGraphicsItem* tpl,
                             QHash<QGraphicsItem*, QString>& nodeTemplateToOrigStableId)
{
    if (!orig || !tpl || isClipboardHandle(orig) || isClipboardHandle(tpl))
        return;
    if (qgraphicsitem_cast<CimdrawConnectLine*>(orig))
        return;

    if (auto* oGrp = dynamic_cast<CimdrawGroup*>(orig))
    {
        auto* tGrp = dynamic_cast<CimdrawGroup*>(tpl);
        if (!tGrp)
            return;
        QList<QGraphicsItem*> origChildren;
        QList<QGraphicsItem*> tplChildren;
        for (QGraphicsItem* c : oGrp->childItems())
        {
            if (!isClipboardHandle(c))
                origChildren.append(c);
        }
        for (QGraphicsItem* c : tGrp->childItems())
        {
            if (!isClipboardHandle(c))
                tplChildren.append(c);
        }
        const int n = qMin(origChildren.size(), tplChildren.size());
        for (int i = 0; i < n; ++i)
            registerNodeTemplateIds(origChildren.at(i), tplChildren.at(i), nodeTemplateToOrigStableId);
        return;
    }

    if (auto* origShape = asStandaloneShape(orig))
    {
        const QString sid = origShape->ensureCimdrawObjectId();
        if (!sid.isEmpty())
            nodeTemplateToOrigStableId.insert(tpl, sid);
    }
}

void rewireConnectLinesTree(QGraphicsItem* tpl,
                            QGraphicsItem* fresh,
                            const QPointF& deltaScene,
                            const QHash<QString, QGraphicsItem*>& origObjectIdToFresh,
                            const QHash<QString, QGraphicsItem*>& topologyBindingIdToFresh,
                            const QHash<QGraphicsItem*, QString>& nodeTemplateToOrigStableId,
                            const QHash<QGraphicsItem*, QPair<QString, QString>>& lineTemplateToOrigEndpointIds,
                            const QHash<QGraphicsItem*, QVector<QPointF>>& lineTemplateScenePaths,
                            const QHash<QGraphicsItem*, ConnectorPathRoutingMode>& lineTemplateRoutingModes)
{
    if (!tpl || !fresh)
        return;

    if (auto* tLine = dynamic_cast<CimdrawConnectLine*>(tpl))
    {
        auto* fLine = dynamic_cast<CimdrawConnectLine*>(fresh);
        if (!fLine)
            return;
        rewireConnectLinePair(tLine, fLine, deltaScene, origObjectIdToFresh, topologyBindingIdToFresh,
                              nodeTemplateToOrigStableId,
                              lineTemplateToOrigEndpointIds,
                              lineTemplateScenePaths, lineTemplateRoutingModes);
        return;
    }

    if (auto* tGrp = dynamic_cast<CimdrawGroup*>(tpl))
    {
        auto* fGrp = dynamic_cast<CimdrawGroup*>(fresh);
        if (!tGrp || !fGrp)
            return;
        QHash<QGraphicsItem*, QGraphicsItem*> tplLineToFreshLine;
        mapGroupConnectLines(tGrp, fGrp, tplLineToFreshLine);
        for (auto it = tplLineToFreshLine.constBegin(); it != tplLineToFreshLine.constEnd(); ++it)
        {
            rewireConnectLinePair(qgraphicsitem_cast<CimdrawConnectLine*>(it.key()),
                                  qgraphicsitem_cast<CimdrawConnectLine*>(it.value()),
                                  deltaScene, origObjectIdToFresh, topologyBindingIdToFresh,
                                  nodeTemplateToOrigStableId,
                                  lineTemplateToOrigEndpointIds,
                                  lineTemplateScenePaths, lineTemplateRoutingModes);
        }
    }
}

void assignFreshObjectIds(QGraphicsItem* item)
{
    if (!item)
        return;
    if (auto* grp = dynamic_cast<CimdrawGroup*>(item))
    {
        grp->setCimdrawObjectId(QString());
        grp->ensureCimdrawObjectId();
        for (QGraphicsItem* child : grp->childItems())
            assignFreshObjectIds(child);
        return;
    }
    if (auto* li = dynamic_cast<CimdrawItem*>(item))
    {
        li->setCimdrawObjectId(QString());
        li->ensureCimdrawObjectId();
    }
}

void registerFreshStableIds(QGraphicsItem* tpl, QGraphicsItem* fresh,
                            const QHash<QGraphicsItem*, QString>& nodeTemplateToOrigStableId,
                            QHash<QString, QGraphicsItem*>& origObjectIdToFresh)
{
    if (!tpl || !fresh)
        return;

    const QString origId = nodeTemplateToOrigStableId.value(tpl);
    if (!origId.isEmpty())
        origObjectIdToFresh.insert(origId, fresh);

    if (auto* tGrp = dynamic_cast<CimdrawGroup*>(tpl))
    {
        auto* fGrp = dynamic_cast<CimdrawGroup*>(fresh);
        if (!fGrp)
            return;
        QList<QGraphicsItem*> tplChildren;
        QList<QGraphicsItem*> freshChildren;
        for (QGraphicsItem* c : tGrp->childItems())
        {
            if (!isClipboardHandle(c))
                tplChildren.append(c);
        }
        for (QGraphicsItem* c : fGrp->childItems())
        {
            if (!isClipboardHandle(c))
                freshChildren.append(c);
        }
        const int n = qMin(tplChildren.size(), freshChildren.size());
        for (int i = 0; i < n; ++i)
            registerFreshStableIds(tplChildren.at(i), freshChildren.at(i),
                                   nodeTemplateToOrigStableId, origObjectIdToFresh);
    }
}

void registerFreshTopologyBindingIds(QGraphicsItem* tpl,
                                     QGraphicsItem* fresh,
                                     QHash<QString, QGraphicsItem*>& topologyBindingIdToFresh)
{
    if (!tpl || !fresh)
        return;

    if (auto* tplShape = asStandaloneShape(tpl))
    {
        const QString topologyBindingId = cimdrawTopologyBindingIdForShape(tplShape);
        if (!topologyBindingId.isEmpty())
            topologyBindingIdToFresh.insert(topologyBindingId, fresh);
    }

    if (auto* tGrp = dynamic_cast<CimdrawGroup*>(tpl))
    {
        auto* fGrp = dynamic_cast<CimdrawGroup*>(fresh);
        if (!fGrp)
            return;
        QList<QGraphicsItem*> tplChildren;
        QList<QGraphicsItem*> freshChildren;
        for (QGraphicsItem* c : tGrp->childItems())
        {
            if (!isClipboardHandle(c))
                tplChildren.append(c);
        }
        for (QGraphicsItem* c : fGrp->childItems())
        {
            if (!isClipboardHandle(c))
                freshChildren.append(c);
        }
        const int n = qMin(tplChildren.size(), freshChildren.size());
        for (int i = 0; i < n; ++i)
            registerFreshTopologyBindingIds(tplChildren.at(i), freshChildren.at(i),
                                            topologyBindingIdToFresh);
    }
}

} // namespace

class CimdrawMimeDataPrivate
{
    Q_DECLARE_PUBLIC(CimdrawMimeData)
public:
    CimdrawMimeDataPrivate(CimdrawMimeData* parent)
        :q_ptr(parent)
    {

    }

    QList<QGraphicsItem*> items;
    QList<QPointF> positions;
    /** 剪贴板模板节点图元 -> 复制时源图元的 cimdrawObjectId（用于粘贴时重绑连线） */
    QHash<QGraphicsItem*, QString> nodeTemplateToOrigStableId;
    /** 剪贴板模板连线 -> 复制时源连线两端节点的稳定 ID */
    QHash<QGraphicsItem*, QPair<QString, QString>> lineTemplateToOrigEndpointIds;
    /** 复制瞬间源连线的场景路径（避免模板 duplicate 后坐标系漂移） */
    QHash<QGraphicsItem*, QVector<QPointF>> lineTemplateScenePaths;
    QHash<QGraphicsItem*, ConnectorPathRoutingMode> lineTemplateRoutingModes;

    CimdrawMimeData* q_ptr;
};

CimdrawMimeData::CimdrawMimeData(QList<QGraphicsItem*> items)
    :d_ptr(new CimdrawMimeDataPrivate(this))
{
    Q_D(CimdrawMimeData);
    const QList<QGraphicsItem*> expanded = expandClipboardItems(items);
    for (auto* item : expanded)
    {
        if (cimdrawIsHandle(item))
            continue;

        if (auto* line = dynamic_cast<CimdrawConnectLine*>(item))
        {
            const QString a = stableEndpointIdForPaste(line, true);
            const QString b = stableEndpointIdForPaste(line, false);
            const QVector<QPointF> scenePath = line->pathInSceneCoords();
            const ConnectorPathRoutingMode routingMode = line->pathRoutingMode();
            QGraphicsItem* duplicatedItem = duplicateClipboardItem(item);
            if (!duplicatedItem)
                continue;
            d->items.append(duplicatedItem);
            d->positions.append(item->pos());
            d->lineTemplateToOrigEndpointIds.insert(duplicatedItem, qMakePair(a, b));
            if (scenePath.size() >= 2)
            {
                d->lineTemplateScenePaths.insert(duplicatedItem, scenePath);
                d->lineTemplateRoutingModes.insert(duplicatedItem, routingMode);
            }
            continue;
        }

        QGraphicsItem* duplicatedItem = duplicateClipboardItem(item);
        if (!duplicatedItem)
            continue;
        d->items.append(duplicatedItem);
        d->positions.append(item->pos());
        registerNodeTemplateIds(item, duplicatedItem, d->nodeTemplateToOrigStableId);
        if (auto* origGrp = asGroupItem(item))
        {
            if (auto* tplGrp = asGroupItem(duplicatedItem))
            {
                registerGroupClipboardLineEndpoints(origGrp, tplGrp,
                                                    d->lineTemplateToOrigEndpointIds,
                                                    d->lineTemplateScenePaths,
                                                    d->lineTemplateRoutingModes);
            }
        }
    }
}

CimdrawMimeData::~CimdrawMimeData()
{
    Q_D(CimdrawMimeData);
    for (auto* item : d->items)
    {
        delete item;
        item = nullptr;
    }
    d->items.clear();
}

QList<QGraphicsItem*> CimdrawMimeData::items() const
{
    Q_D(const CimdrawMimeData);
    return d->items;
}

QList<QPointF> CimdrawMimeData::delta() const
{
    Q_D(const CimdrawMimeData);
    return d->positions;
}

QList<QGraphicsItem*> CimdrawMimeData::instantiatePasteBatch(const QPointF& deltaScene) const
{
    Q_D(const CimdrawMimeData);
    QList<QGraphicsItem*> freshList;
    QVector<QPair<QGraphicsItem*, QGraphicsItem*>> tplToFresh;
    tplToFresh.reserve(d->items.size());

    QHash<QString, QGraphicsItem*> origObjectIdToFresh;
    QHash<QString, QGraphicsItem*> topologyBindingIdToFresh;

    for (int i = 0; i < d->items.size(); ++i)
    {
        QGraphicsItem* tpl = d->items.at(i);
        QGraphicsItem* fresh = duplicateClipboardItem(tpl);
        if (!fresh)
            continue;
        freshList.append(fresh);
        tplToFresh.append(qMakePair(tpl, fresh));

        assignFreshObjectIds(fresh);

        registerFreshStableIds(tpl, fresh, d->nodeTemplateToOrigStableId, origObjectIdToFresh);
        registerFreshTopologyBindingIds(tpl, fresh, topologyBindingIdToFresh);
    }

    for (const auto& pr : tplToFresh)
    {
        QGraphicsItem* tpl = pr.first;
        QGraphicsItem* fresh = pr.second;
        if (!tpl || !fresh)
            continue;
        fresh->setPos(tpl->pos() + deltaScene);
        if (auto* fGrp = dynamic_cast<CimdrawGroup*>(fresh))
        {
            if (auto* tGrp = dynamic_cast<CimdrawGroup*>(tpl))
                fGrp->copyItemFrameFrom(tGrp);
        }
    }

    for (const auto& pr : tplToFresh)
    {
        rewireConnectLinesTree(pr.first, pr.second, deltaScene, origObjectIdToFresh,
                               topologyBindingIdToFresh,
                               d->nodeTemplateToOrigStableId, d->lineTemplateToOrigEndpointIds,
                               d->lineTemplateScenePaths,
                               d->lineTemplateRoutingModes);
    }

    for (const auto& pr : tplToFresh)
    {
        if (auto* fGrp = dynamic_cast<CimdrawGroup*>(pr.second))
            CimdrawGroup::registerConnectLinesAmongItems(fGrp->childItems());
    }

    return freshList;
}
