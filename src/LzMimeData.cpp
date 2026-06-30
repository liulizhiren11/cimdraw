#include "LzMimeData.h"
#include "item/TmpBase.h"
#include "item/LzConnectLine.h"
#include "item/LzConnectPoint.h"
#include "item/LzGroup.h"
#include "item/LzItem.h"
#include "LzPowerBusbarSectionItem.h"
#include "LzConnectConfig.h"

#include <QGraphicsScene>
#include <QHash>
#include <QPair>
#include <QSet>

namespace {

bool isClipboardHandle(const QGraphicsItem* item)
{
    return item && item->type() == LzHandle::Type;
}

LzItem* asStandaloneShape(QGraphicsItem* item)
{
    return dynamic_cast<LzItem*>(item);
}

LzGroup* asGroupItem(QGraphicsItem* item)
{
    return dynamic_cast<LzGroup*>(item);
}

QGraphicsItem* duplicateClipboardItem(QGraphicsItem* item)
{
    if (auto* grp = asGroupItem(item))
        return grp->duplicate();
    if (auto* shape = asStandaloneShape(item))
        return shape->duplicate();
    return nullptr;
}

int connectPointOrdinalOnItem(QGraphicsItem* item, LzConnectPoint* port)
{
    if (!item || !port)
        return -1;
    int ordinal = 0;
    for (QGraphicsItem* child : item->childItems())
    {
        if (auto* candidate = qgraphicsitem_cast<LzConnectPoint*>(child))
        {
            if (candidate == port)
                return ordinal;
            ++ordinal;
        }
    }
    return -1;
}

LzConnectPoint* connectPointByOrdinal(QGraphicsItem* item, int ordinal)
{
    if (!item || ordinal < 0)
        return nullptr;
    int current = 0;
    for (QGraphicsItem* child : item->childItems())
    {
        if (auto* port = qgraphicsitem_cast<LzConnectPoint*>(child))
        {
            if (current == ordinal)
                return port;
            ++current;
        }
    }
    return nullptr;
}

struct PasteEndpointPortHint
{
    QPointF startPortScene;
    QPointF endPortScene;
    bool hasStartPortScene = false;
    bool hasEndPortScene = false;
};

LzConnectPoint* resolvePasteConnectPort(QGraphicsItem* item,
                                        int ordinal,
                                        const QPointF& portSceneHint,
                                        bool hasPortSceneHint)
{
    if (!item || ordinal < 0)
        return nullptr;
    if (auto* bus = dynamic_cast<LzPowerBusbarSectionItem*>(item))
    {
        if (hasPortSceneHint)
        {
            if (LzConnectPoint* port = bus->findConnectPortNearScene(portSceneHint, 64.0))
                return port;
            if (LzConnectPoint* port = bus->ensureConnectPointAtScene(portSceneHint))
                return port;
        }
        return connectPointByOrdinal(item, ordinal);
    }
    return connectPointByOrdinal(item, ordinal);
}

QString stableNodeIdForPaste(QGraphicsItem* it)
{
    if (!it)
        return {};
    if (auto* shape = asStandaloneShape(it))
        return shape->ensureLzObjectId();
    return {};
}

void collectShapeLeaves(QGraphicsItem* root, QSet<QGraphicsItem*>& shapes)
{
    if (!root || isClipboardHandle(root))
        return;
    if (auto* grp = dynamic_cast<LzGroup*>(root))
    {
        for (QGraphicsItem* child : grp->childItems())
            collectShapeLeaves(child, shapes);
        return;
    }
    if (qgraphicsitem_cast<LzConnectLine*>(root))
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
        auto* grp = dynamic_cast<LzGroup*>(top);
        if (!grp)
            continue;
        for (QGraphicsItem* child : grp->childItems())
        {
            if (auto* line = qgraphicsitem_cast<LzConnectLine*>(child))
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
        if (auto* grp = dynamic_cast<LzGroup*>(item))
        {
            if (!result.contains(grp))
                result.append(grp);
            topGroups.append(grp);
            collectShapeLeaves(grp, shapeScope);
            collectConnectLinesInsideGroups({grp}, linesInsideSelectedGroups);
            continue;
        }
        if (auto* line = qgraphicsitem_cast<LzConnectLine*>(item))
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
            auto* line = qgraphicsitem_cast<LzConnectLine*>(si);
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

void collectGroupChildrenInOrder(const LzGroup* group, QList<QGraphicsItem*>& out)
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

void collectGroupChildLinesInOrder(const LzGroup* group, QList<LzConnectLine*>& out)
{
    out.clear();
    if (!group)
        return;
    for (QGraphicsItem* child : group->childItems())
    {
        if (isClipboardHandle(child))
            continue;
        if (auto* line = qgraphicsitem_cast<LzConnectLine*>(child))
            out.append(line);
    }
}

QString stableEndpointIdForPaste(LzConnectLine* line, bool startEndpoint)
{
    if (!line)
        return {};
    QString id = stableNodeIdForPaste(startEndpoint ? line->getStartItem() : line->getEndItem());
    if (!id.isEmpty())
        return id;
    return startEndpoint ? line->topologyStartNodeStableId() : line->topologyEndNodeStableId();
}

void mapGroupConnectLines(LzGroup* tplGrp,
                          LzGroup* freshGrp,
                          QHash<QGraphicsItem*, QGraphicsItem*>& outTplLineToFreshLine)
{
    outTplLineToFreshLine.clear();
    if (!tplGrp || !freshGrp)
        return;
    QList<LzConnectLine*> tplLines;
    QList<LzConnectLine*> freshLines;
    collectGroupChildLinesInOrder(tplGrp, tplLines);
    collectGroupChildLinesInOrder(freshGrp, freshLines);
    const int n = qMin(tplLines.size(), freshLines.size());
    for (int i = 0; i < n; ++i)
        outTplLineToFreshLine.insert(tplLines.at(i), freshLines.at(i));
}

void registerGroupClipboardLineEndpoints(LzGroup* origGrp,
                                       LzGroup* tplGrp,
                                       QHash<QGraphicsItem*, QPair<QString, QString>>& lineTemplateToOrigEndpointIds,
                                       QHash<QGraphicsItem*, QPair<int, int>>& lineTemplateToEndpointOrdinals,
                                       QHash<QGraphicsItem*, QVector<QPointF>>& lineTemplateScenePaths,
                                       QHash<QGraphicsItem*, ConnectorPathRoutingMode>& lineTemplateRoutingModes,
                                       QHash<QGraphicsItem*, PasteEndpointPortHint>& lineTemplateToEndpointPortHints)
{
    if (!origGrp || !tplGrp)
        return;

    QList<LzConnectLine*> origLines;
    QList<LzConnectLine*> tplLines;
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
        lineTemplateToEndpointOrdinals.insert(tLine,
                                              qMakePair(
                                                  connectPointOrdinalOnItem(oLine->getStartItem(),
                                                                          oLine->startConnectPort()),
                                                  connectPointOrdinalOnItem(oLine->getEndItem(),
                                                                          oLine->endConnectPort())));
        lineTemplateScenePaths.insert(tLine, scenePath);
        lineTemplateRoutingModes.insert(tLine, oLine->pathRoutingMode());
        PasteEndpointPortHint portHint;
        if (LzConnectPoint* sp = oLine->startConnectPort())
        {
            portHint.hasStartPortScene = true;
            portHint.startPortScene = sp->connectionCenterInScene();
        }
        if (LzConnectPoint* ep = oLine->endConnectPort())
        {
            portHint.hasEndPortScene = true;
            portHint.endPortScene = ep->connectionCenterInScene();
        }
        lineTemplateToEndpointPortHints.insert(tLine, portHint);
    }
}

bool rewireConnectLinePair(LzConnectLine* tplLine,
                           LzConnectLine* freshLine,
                           const QPointF& deltaScene,
                           const QHash<QString, QGraphicsItem*>& origStableIdToFresh,
                           const QHash<QGraphicsItem*, QString>& nodeTemplateToOrigStableId,
                           const QHash<QGraphicsItem*, QPair<QString, QString>>& lineTemplateToOrigEndpointIds,
                           const QHash<QGraphicsItem*, QPair<int, int>>& lineTemplateToEndpointOrdinals,
                           const QHash<QGraphicsItem*, QVector<QPointF>>& lineTemplateScenePaths,
                           const QHash<QGraphicsItem*, ConnectorPathRoutingMode>& lineTemplateRoutingModes,
                           const QHash<QGraphicsItem*, PasteEndpointPortHint>& lineTemplateToEndpointPortHints)
{
    if (!tplLine || !freshLine)
        return false;

    QString startId;
    QString endId;
    QPair<int, int> portOrdinals(-1, -1);
    if (const auto ends = lineTemplateToOrigEndpointIds.constFind(tplLine);
        ends != lineTemplateToOrigEndpointIds.constEnd())
    {
        startId = ends.value().first;
        endId = ends.value().second;
        portOrdinals = lineTemplateToEndpointOrdinals.value(tplLine, qMakePair(-1, -1));
    }
    else
    {
        startId = nodeTemplateToOrigStableId.value(tplLine->getStartItem());
        endId = nodeTemplateToOrigStableId.value(tplLine->getEndItem());
        portOrdinals = qMakePair(connectPointOrdinalOnItem(tplLine->getStartItem(), tplLine->startConnectPort()),
                                 connectPointOrdinalOnItem(tplLine->getEndItem(), tplLine->endConnectPort()));
    }
    if (startId.isEmpty())
        startId = tplLine->topologyStartNodeStableId();
    if (endId.isEmpty())
        endId = tplLine->topologyEndNodeStableId();
    if (startId.isEmpty() || endId.isEmpty())
        return false;

    QGraphicsItem* startFresh = origStableIdToFresh.value(startId);
    QGraphicsItem* endFresh = origStableIdToFresh.value(endId);
    if (!startFresh)
        startFresh = origStableIdToFresh.value(tplLine->topologyStartNodeStableId());
    if (!endFresh)
        endFresh = origStableIdToFresh.value(tplLine->topologyEndNodeStableId());
    if (!startFresh || !endFresh)
    {
        qWarning().noquote()
            << "rewireConnectLinePair failed:"
            << "startId=" << startId
            << "endId=" << endId
            << "tplStartTopo=" << tplLine->topologyStartNodeStableId()
            << "tplEndTopo=" << tplLine->topologyEndNodeStableId()
            << "hasStartFresh=" << (startFresh != nullptr)
            << "hasEndFresh=" << (endFresh != nullptr)
            << "mapKeys=" << origStableIdToFresh.keys();
        return false;
    }

    freshLine->setStartItem(startFresh);
    freshLine->setEndItem(endFresh);
    freshLine->setStartConnectPort(resolvePasteConnectPort(startFresh,
                                                          portOrdinals.first,
                                                          lineTemplateToEndpointPortHints.value(tplLine).startPortScene,
                                                          lineTemplateToEndpointPortHints.value(tplLine).hasStartPortScene));
    freshLine->setEndConnectPort(resolvePasteConnectPort(endFresh,
                                                        portOrdinals.second,
                                                        lineTemplateToEndpointPortHints.value(tplLine).endPortScene,
                                                        lineTemplateToEndpointPortHints.value(tplLine).hasEndPortScene));

    QVector<QPointF> scenePath = lineTemplateScenePaths.value(tplLine);
    if (scenePath.size() < 2)
        scenePath = tplLine->pathInSceneCoords();
    PasteEndpointPortHint portHint = lineTemplateToEndpointPortHints.value(tplLine);
    for (QPointF& p : scenePath)
        p += deltaScene;
    if (portHint.hasStartPortScene)
        portHint.startPortScene += deltaScene;
    if (portHint.hasEndPortScene)
        portHint.endPortScene += deltaScene;
    if (scenePath.size() < 2)
    {
        const QPointF startScene = startFresh->mapToScene(startFresh->boundingRect().center());
        const QPointF endScene = endFresh->mapToScene(endFresh->boundingRect().center());
        scenePath = {startScene, endScene};
    }
    const ConnectorPathRoutingMode routingMode =
        lineTemplateRoutingModes.value(tplLine, tplLine->pathRoutingMode());

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
    if (qgraphicsitem_cast<LzConnectLine*>(orig))
        return;

    if (auto* oGrp = dynamic_cast<LzGroup*>(orig))
    {
        auto* tGrp = dynamic_cast<LzGroup*>(tpl);
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
        const QString sid = origShape->ensureLzObjectId();
        if (!sid.isEmpty())
            nodeTemplateToOrigStableId.insert(tpl, sid);
    }
}

void rewireConnectLinesTree(QGraphicsItem* tpl,
                            QGraphicsItem* fresh,
                            const QPointF& deltaScene,
                            const QHash<QString, QGraphicsItem*>& origStableIdToFresh,
                            const QHash<QGraphicsItem*, QString>& nodeTemplateToOrigStableId,
                            const QHash<QGraphicsItem*, QPair<QString, QString>>& lineTemplateToOrigEndpointIds,
                            const QHash<QGraphicsItem*, QPair<int, int>>& lineTemplateToEndpointOrdinals,
                            const QHash<QGraphicsItem*, QVector<QPointF>>& lineTemplateScenePaths,
                            const QHash<QGraphicsItem*, ConnectorPathRoutingMode>& lineTemplateRoutingModes,
                            const QHash<QGraphicsItem*, PasteEndpointPortHint>& lineTemplateToEndpointPortHints)
{
    if (!tpl || !fresh)
        return;

    if (auto* tLine = dynamic_cast<LzConnectLine*>(tpl))
    {
        auto* fLine = dynamic_cast<LzConnectLine*>(fresh);
        if (!fLine)
            return;
        rewireConnectLinePair(tLine, fLine, deltaScene, origStableIdToFresh, nodeTemplateToOrigStableId,
                              lineTemplateToOrigEndpointIds, lineTemplateToEndpointOrdinals,
                              lineTemplateScenePaths, lineTemplateRoutingModes,
                              lineTemplateToEndpointPortHints);
        return;
    }

    if (auto* tGrp = dynamic_cast<LzGroup*>(tpl))
    {
        auto* fGrp = dynamic_cast<LzGroup*>(fresh);
        if (!tGrp || !fGrp)
            return;
        QHash<QGraphicsItem*, QGraphicsItem*> tplLineToFreshLine;
        mapGroupConnectLines(tGrp, fGrp, tplLineToFreshLine);
        for (auto it = tplLineToFreshLine.constBegin(); it != tplLineToFreshLine.constEnd(); ++it)
        {
            rewireConnectLinePair(qgraphicsitem_cast<LzConnectLine*>(it.key()),
                                  qgraphicsitem_cast<LzConnectLine*>(it.value()),
                                  deltaScene, origStableIdToFresh, nodeTemplateToOrigStableId,
                                  lineTemplateToOrigEndpointIds, lineTemplateToEndpointOrdinals,
                                  lineTemplateScenePaths, lineTemplateRoutingModes,
                                  lineTemplateToEndpointPortHints);
        }
    }
}

void assignFreshObjectIds(QGraphicsItem* item)
{
    if (!item)
        return;
    if (auto* grp = dynamic_cast<LzGroup*>(item))
    {
        grp->setLzObjectId(QString());
        grp->ensureLzObjectId();
        for (QGraphicsItem* child : grp->childItems())
            assignFreshObjectIds(child);
        return;
    }
    if (auto* li = dynamic_cast<LzItem*>(item))
    {
        li->setLzObjectId(QString());
        li->ensureLzObjectId();
    }
}

void registerFreshStableIds(QGraphicsItem* tpl, QGraphicsItem* fresh,
                            const QHash<QGraphicsItem*, QString>& nodeTemplateToOrigStableId,
                            QHash<QString, QGraphicsItem*>& origStableIdToFresh)
{
    if (!tpl || !fresh)
        return;

    const QString origId = nodeTemplateToOrigStableId.value(tpl);
    if (!origId.isEmpty())
        origStableIdToFresh.insert(origId, fresh);

    if (auto* freshShape = asStandaloneShape(fresh))
    {
        const QString topoId = freshShape->topologyNodeStableId();
        if (!topoId.isEmpty() && !origStableIdToFresh.contains(topoId))
            origStableIdToFresh.insert(topoId, fresh);
    }

    if (auto* tGrp = dynamic_cast<LzGroup*>(tpl))
    {
        auto* fGrp = dynamic_cast<LzGroup*>(fresh);
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
                                   nodeTemplateToOrigStableId, origStableIdToFresh);
    }
}

} // namespace

class LzMimeDataPrivate
{
    Q_DECLARE_PUBLIC(LzMimeData)
public:
    LzMimeDataPrivate(LzMimeData* parent)
        :q_ptr(parent)
    {

    }

    QList<QGraphicsItem*> items;
    QList<QPointF> positions;
    /** 剪贴板模板节点图元 -> 复制时源图元的 lzObjectId（用于粘贴时重绑连线） */
    QHash<QGraphicsItem*, QString> nodeTemplateToOrigStableId;
    /** 剪贴板模板连线 -> 复制时源连线两端节点的稳定 ID */
    QHash<QGraphicsItem*, QPair<QString, QString>> lineTemplateToOrigEndpointIds;
    /** 剪贴板模板连线 -> 复制时源连线两端连接点序号 */
    QHash<QGraphicsItem*, QPair<int, int>> lineTemplateToEndpointOrdinals;
    /** 复制瞬间源连线的场景路径（避免模板 duplicate 后坐标系漂移） */
    QHash<QGraphicsItem*, QVector<QPointF>> lineTemplateScenePaths;
    QHash<QGraphicsItem*, ConnectorPathRoutingMode> lineTemplateRoutingModes;
    QHash<QGraphicsItem*, PasteEndpointPortHint> lineTemplateToEndpointPortHints;

    LzMimeData* q_ptr;
};

LzMimeData::LzMimeData(QList<QGraphicsItem*> items)
    :d_ptr(new LzMimeDataPrivate(this))
{
    Q_D(LzMimeData);
    const QList<QGraphicsItem*> expanded = expandClipboardItems(items);
    for (auto* item : expanded)
    {
        if (lzIsHandle(item))
            continue;

        if (auto* line = dynamic_cast<LzConnectLine*>(item))
        {
            const QString a = stableEndpointIdForPaste(line, true);
            const QString b = stableEndpointIdForPaste(line, false);
            const int startOrdinal = connectPointOrdinalOnItem(line->getStartItem(), line->startConnectPort());
            const int endOrdinal = connectPointOrdinalOnItem(line->getEndItem(), line->endConnectPort());
            const QVector<QPointF> scenePath = line->pathInSceneCoords();
            const ConnectorPathRoutingMode routingMode = line->pathRoutingMode();
            QGraphicsItem* duplicatedItem = duplicateClipboardItem(item);
            if (!duplicatedItem)
                continue;
            d->items.append(duplicatedItem);
            d->positions.append(item->pos());
            d->lineTemplateToOrigEndpointIds.insert(duplicatedItem, qMakePair(a, b));
            d->lineTemplateToEndpointOrdinals.insert(duplicatedItem, qMakePair(startOrdinal, endOrdinal));
            if (scenePath.size() >= 2)
            {
                d->lineTemplateScenePaths.insert(duplicatedItem, scenePath);
                d->lineTemplateRoutingModes.insert(duplicatedItem, routingMode);
            }
            PasteEndpointPortHint portHint;
            if (LzConnectPoint* sp = line->startConnectPort())
            {
                portHint.hasStartPortScene = true;
                portHint.startPortScene = sp->connectionCenterInScene();
            }
            if (LzConnectPoint* ep = line->endConnectPort())
            {
                portHint.hasEndPortScene = true;
                portHint.endPortScene = ep->connectionCenterInScene();
            }
            d->lineTemplateToEndpointPortHints.insert(duplicatedItem, portHint);
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
                                                    d->lineTemplateToEndpointOrdinals,
                                                    d->lineTemplateScenePaths,
                                                    d->lineTemplateRoutingModes,
                                                    d->lineTemplateToEndpointPortHints);
            }
        }
    }
}

LzMimeData::~LzMimeData()
{
    Q_D(LzMimeData);
    for (auto* item : d->items)
    {
        delete item;
        item = nullptr;
    }
    d->items.clear();
}

QList<QGraphicsItem*> LzMimeData::items() const
{
    Q_D(const LzMimeData);
    return d->items;
}

QList<QPointF> LzMimeData::delta() const
{
    Q_D(const LzMimeData);
    return d->positions;
}

QList<QGraphicsItem*> LzMimeData::instantiatePasteBatch(const QPointF& deltaScene) const
{
    Q_D(const LzMimeData);
    QList<QGraphicsItem*> freshList;
    QVector<QPair<QGraphicsItem*, QGraphicsItem*>> tplToFresh;
    tplToFresh.reserve(d->items.size());

    QHash<QString, QGraphicsItem*> origStableIdToFresh;

    for (int i = 0; i < d->items.size(); ++i)
    {
        QGraphicsItem* tpl = d->items.at(i);
        QGraphicsItem* fresh = duplicateClipboardItem(tpl);
        if (!fresh)
            continue;
        freshList.append(fresh);
        tplToFresh.append(qMakePair(tpl, fresh));

        assignFreshObjectIds(fresh);

        registerFreshStableIds(tpl, fresh, d->nodeTemplateToOrigStableId, origStableIdToFresh);
    }

    for (const auto& pr : tplToFresh)
    {
        QGraphicsItem* tpl = pr.first;
        QGraphicsItem* fresh = pr.second;
        if (!tpl || !fresh)
            continue;
        fresh->setPos(tpl->pos() + deltaScene);
        if (auto* fGrp = dynamic_cast<LzGroup*>(fresh))
        {
            if (auto* tGrp = dynamic_cast<LzGroup*>(tpl))
                fGrp->copyItemFrameFrom(tGrp);
        }
    }

    for (const auto& pr : tplToFresh)
    {
        rewireConnectLinesTree(pr.first, pr.second, deltaScene, origStableIdToFresh,
                               d->nodeTemplateToOrigStableId, d->lineTemplateToOrigEndpointIds,
                               d->lineTemplateToEndpointOrdinals, d->lineTemplateScenePaths,
                               d->lineTemplateRoutingModes, d->lineTemplateToEndpointPortHints);
    }

    for (const auto& pr : tplToFresh)
    {
        if (auto* fGrp = dynamic_cast<LzGroup*>(pr.second))
            LzGroup::registerConnectLinesAmongItems(fGrp->childItems());
    }

    return freshList;
}
