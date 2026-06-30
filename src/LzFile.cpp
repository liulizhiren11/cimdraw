#include <QFileInfo>
#include <QTextStream>
#include <QDomProcessingInstruction>
#include <QFile>
#include <QColor>
#include <QLineF>

#include "LzFile.h"
#include "LzScene.h"
#include "LzView.h"
#include "Item/TmpBase.h"
#include "Item/LzItem.h"
#include "Item/LzGroup.h"
#include "item/LzConnectLine.h"
#include "item/LzConnectPoint.h"
#include "item/LzObjectFactory.h"
#include "wiring/base/LzWiringItemBase.h"
#include "wiring/power/LzPowerBusbarSectionItem.h"

#include <limits>

class LzFilePrivate
{
    Q_DECLARE_PUBLIC(LzFile)
public:
    LzFilePrivate(LzFile* file)
        :q_ptr(file)
    {

    }
    LzFile* q_ptr;
    QString filePath;
};

namespace {
void ensureSavableItemStableIds(const QList<QGraphicsItem*>& items);
LzConnectPoint* firstConnectPointOnItem(QGraphicsItem* item);
LzConnectPoint* connectPointByOrdinalOnItem(QGraphicsItem* item, int ordinal);
int connectPointOrdinalOnItem(QGraphicsItem* item, LzConnectPoint* port);
int parsePortOrdinal(const QString& key);
LzItem* findNearestBindableItem(LzScene* scene, const QPointF& scenePos, QGraphicsItem* excludeItem = nullptr);
}

LzFile::LzFile(const QString& path)
    :d_ptr(new LzFilePrivate(this))
{
    d_ptr->filePath = path;
}

LzFile::~LzFile()
{

}

bool LzFile::save(LzScene* scene)
{
    SAVE_FILE_SUFFIX type = getFileTypeByName(d_ptr->filePath);
    switch(type)
    {
    case SAVE_FILE_SUFFIX::LZ_XML:
    case SAVE_FILE_SUFFIX::LZ_PDW:
    {
        return saveAsXml(scene);
        break;
    }
    default:
        return false;
    }
}

bool LzFile::saveAsXml(LzScene* scene)
{
    QFile f(d_ptr->filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }
    QTextStream ts(&f);
    QDomDocument doc("LzDraw");
    QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);

    QDomElement root = doc.createElement("LzFile");
    doc.appendChild(root);
    QDomElement header = doc.createElement("Header");
    QColor canvasColor = scene->backgroundBrush().color();
    if (LzView* view = scene->getView())
    {
        const QColor viewColor = view->getBackgroundCol();
        if (viewColor.isValid())
            canvasColor = viewColor;
    }
    if (!canvasColor.isValid())
        canvasColor = QColor(Qt::white);
    header.setAttribute("canvascolor", canvasColor.name());
    //TODO:如果有背景图片
    // header.setAttribute("image", scene->getView()->getBackgroundImage());

    root.appendChild(header);
    QList<QGraphicsItem*> objects = scene->items();
    ensureSavableItemStableIds(objects);
    QDomElement le = doc.createElement("item");
    root.appendChild(le);
    //TODO:连接线是否作为与图元一样的保存方式
    for (QGraphicsItem* obj : objects)
    {
        if (obj->parentItem() && obj->type() < QGraphicsItem::UserType)
            continue;

        QDomElement oe = doc.createElement(QStringLiteral("Object"));
        bool ok = false;
        if (auto* li = dynamic_cast<LzItem*>(obj))
            ok = li->saveXml(&oe);
        else if (auto* grp = dynamic_cast<LzGroup*>(obj))
            ok = grp->saveXml(&oe);
        else
            continue;

        if (ok)
            le.appendChild(oe);
    }
    f.resize(0);
    doc.save(ts, 4);
    f.close();
    return true;
}

namespace {

void ensureSavableItemStableIds(const QList<QGraphicsItem*>& items)
{
    for (QGraphicsItem* item : items)
    {
        if (!item)
            continue;
        if (auto* grp = dynamic_cast<LzGroup*>(item))
        {
            grp->ensureLzObjectId();
            continue;
        }
        if (auto* li = dynamic_cast<LzItem*>(item))
            li->ensureLzObjectId();
    }
}

LzConnectPoint* firstConnectPointOnItem(QGraphicsItem* item)
{
    if (!item)
        return nullptr;
    for (QGraphicsItem* child : item->childItems())
    {
        if (auto* port = qgraphicsitem_cast<LzConnectPoint*>(child))
            return port;
    }
    return nullptr;
}

LzConnectPoint* connectPointByOrdinalOnItem(QGraphicsItem* item, int ordinal)
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

int parsePortOrdinal(const QString& key)
{
    if (key.isEmpty())
        return -1;
    bool ok = false;
    const int ordinal = key.toInt(&ok);
    return ok ? ordinal : -1;
}

LzConnectPoint* nearestConnectPointOnItem(QGraphicsItem* item, const QPointF& scenePos)
{
    if (!item)
        return nullptr;
    LzConnectPoint* best = nullptr;
    qreal bestDist = std::numeric_limits<qreal>::max();
    for (QGraphicsItem* child : item->childItems())
    {
        auto* port = qgraphicsitem_cast<LzConnectPoint*>(child);
        if (!port)
            continue;
        const qreal dist = QLineF(port->connectionCenterInScene(), scenePos).length();
        if (!best || dist < bestDist)
        {
            best = port;
            bestDist = dist;
        }
    }
    return best;
}

LzItem* findNearestBindableItem(LzScene* scene, const QPointF& scenePos, QGraphicsItem* excludeItem)
{
    if (!scene)
        return nullptr;

    LzItem* best = nullptr;
    qreal bestDist = std::numeric_limits<qreal>::max();
    for (QGraphicsItem* item : scene->items())
    {
        auto* shape = dynamic_cast<LzItem*>(item);
        if (!shape || shape == excludeItem)
            continue;

        qreal candidateDist = std::numeric_limits<qreal>::max();
        bool hasPort = false;
        for (QGraphicsItem* child : shape->childItems())
        {
            auto* port = qgraphicsitem_cast<LzConnectPoint*>(child);
            if (!port)
                continue;
            hasPort = true;
            candidateDist = qMin(candidateDist, QLineF(port->connectionCenterInScene(), scenePos).length());
        }

        if (!hasPort)
        {
            const QPointF center = shape->mapToScene(shape->boundingRect().center());
            candidateDist = QLineF(center, scenePos).length();
        }

        if (!best || candidateDist < bestDist)
        {
            best = shape;
            bestDist = candidateDist;
        }
    }
    return best;
}

void clearSavableTopLevelItems(LzScene* scene)
{
    if (!scene)
        return;
    const QList<QGraphicsItem*> all = scene->items();
    QList<QGraphicsItem*> removeList;
    for (QGraphicsItem* gi : all) {
        if (gi->parentItem() != nullptr)
            continue;
        if (dynamic_cast<LzItem*>(gi) != nullptr || dynamic_cast<LzGroup*>(gi) != nullptr)
            removeList.append(gi);
    }
    for (QGraphicsItem* gi : removeList)
        delete gi;
}

LzItem* findItemByStableId(LzScene* scene, const QString& stableId)
{
    if (!scene || stableId.isEmpty())
        return nullptr;
    for (QGraphicsItem* item : scene->items())
    {
        auto* shape = dynamic_cast<LzItem*>(item);
        if (shape && shape->topologyNodeStableId() == stableId)
            return shape;
    }
    return nullptr;
}

void rebindLoadedConnectLines(LzScene* scene)
{
    if (!scene)
        return;

    for (QGraphicsItem* item : scene->items())
    {
        auto* line = qgraphicsitem_cast<LzConnectLine*>(item);
        if (!line)
            continue;

        const QString savedStartPortKey = line->topologyStartPortKey();
        const QString savedEndPortKey = line->topologyEndPortKey();
        const ConnectorPathRoutingMode savedRoutingMode = line->pathRoutingMode();
        const QVector<QPointF> savedPath = line->pathInSceneCoords();
        const QPointF startAnchor = savedPath.isEmpty() ? QPointF() : savedPath.first();
        const QPointF endAnchor = savedPath.size() < 2 ? QPointF() : savedPath.last();

        if (!line->getStartItem())
            line->setStartItem(findItemByStableId(scene, line->topologyStartNodeStableId()));
        if (!line->getEndItem())
            line->setEndItem(findItemByStableId(scene, line->topologyEndNodeStableId()));

        if (!line->getStartItem() && savedPath.size() >= 2)
            line->setStartItem(findNearestBindableItem(scene, startAnchor, line->getEndItem()));
        if (!line->getEndItem() && savedPath.size() >= 2)
            line->setEndItem(findNearestBindableItem(scene, endAnchor, line->getStartItem()));

        if (!line->getStartItem() || !line->getEndItem())
            continue;

        line->setTopologyStartPortKey(savedStartPortKey);
        line->setTopologyEndPortKey(savedEndPortKey);

        const QVector<QPointF> path = savedPath;
        if (path.size() >= 2)
        {
            if (!line->startConnectPort())
            {
                const int ordinal = parsePortOrdinal(savedStartPortKey);
                if (ordinal >= 0)
                    line->setStartConnectPort(connectPointByOrdinalOnItem(line->getStartItem(), ordinal));
                if (!line->startConnectPort())
                {
                    if (auto* bus = dynamic_cast<LzPowerBusbarSectionItem*>(line->getStartItem()))
                        line->setStartConnectPort(bus->ensureConnectPointAtScene(path.first()));
                    else if (auto* wiring = dynamic_cast<LzWiringItemBase*>(line->getStartItem()))
                    {
                        line->setStartConnectPort(nearestConnectPointOnItem(line->getStartItem(), path.first()));
                        if (!line->startConnectPort())
                            line->setStartConnectPort(wiring->ensureConnectPointAtScene(path.first()));
                    }
                }
                if (!line->startConnectPort())
                    line->setStartConnectPort(firstConnectPointOnItem(line->getStartItem()));
            }
            if (!line->endConnectPort())
            {
                const int ordinal = parsePortOrdinal(savedEndPortKey);
                if (ordinal >= 0)
                    line->setEndConnectPort(connectPointByOrdinalOnItem(line->getEndItem(), ordinal));
                if (!line->endConnectPort())
                {
                    if (auto* bus = dynamic_cast<LzPowerBusbarSectionItem*>(line->getEndItem()))
                        line->setEndConnectPort(bus->ensureConnectPointAtScene(path.last()));
                    else if (auto* wiring = dynamic_cast<LzWiringItemBase*>(line->getEndItem()))
                    {
                        line->setEndConnectPort(nearestConnectPointOnItem(line->getEndItem(), path.last()));
                        if (!line->endConnectPort())
                            line->setEndConnectPort(wiring->ensureConnectPointAtScene(path.last()));
                    }
                }
                if (!line->endConnectPort())
                    line->setEndConnectPort(firstConnectPointOnItem(line->getEndItem()));
            }
        }

        line->registerEndpointAttachments();
        line->restorePathSnapshot(savedPath, savedRoutingMode);
    }
}

} // namespace

bool LzFile::savePdw(LzScene* scene)
{
    return saveAsXml(scene);
}

bool LzFile::loadPdw(const QString& filePath, LzScene* scene, QString* errMsg)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errMsg)
            *errMsg = QStringLiteral("无法打开文件");
        return false;
    }
    const QByteArray data = f.readAll();
    f.close();
    QDomDocument doc;
    QString parseError;
    int el = 0;
    int ec = 0;
    if (!doc.setContent(data, &parseError, &el, &ec)) {
        if (errMsg)
            *errMsg = QStringLiteral("%1 (%2,%3)").arg(parseError).arg(el).arg(ec);
        return false;
    }
    LzFile lf(filePath);
    return lf.loadByXml(&doc, scene);
}

bool LzFile::load(QDomDocument* dom,QString* errMsg)
{
    return true;
}

bool LzFile::loadByXml(QDomDocument* dom, LzScene* scene)
{
    if (!dom || !scene)
    {
        return false;
    }

    QDomElement docElem = dom->documentElement();
    if (docElem.tagName() != QLatin1String("LzFile"))
    {
        return false;
    }

    QDomNode node = docElem.firstChild();
    while (!node.isNull())
    {
        QDomElement e = node.toElement();
        if (!e.isNull())
        {
            if (e.tagName() == QLatin1String("Header"))
            {
                const QString attrClr = e.attribute(QStringLiteral("canvascolor"));
                if (!attrClr.isEmpty())
                {
                    const QColor color(attrClr);
                    scene->setBackgroundBrush(color);
                    if (LzView* view = scene->getView())
                        view->setBackgroundCol(color);
                }
            }
        }
        node = node.nextSibling();
    }

    const QDomElement itemBucket = docElem.firstChildElement(QStringLiteral("item"));
    if (itemBucket.isNull())
        return true;

    clearSavableTopLevelItems(scene);

    QDomElement objEl = itemBucket.firstChildElement(QStringLiteral("Object"));
    while (!objEl.isNull()) {
        toObject(scene, &objEl, true);
        objEl = objEl.nextSiblingElement(QStringLiteral("Object"));
    }
    rebindLoadedConnectLines(scene);
    scene->rebuildTopologyIndex();
    return true;
}

SAVE_FILE_SUFFIX LzFile::getFileTypeByName(const QString& fileName)
{
    QFileInfo fileInfo(fileName);
    QString suffix = fileInfo.suffix().toLower();

    if (suffix == QLatin1String("xml") || suffix == QLatin1String("pdw"))
    {
        if (suffix == QLatin1String("pdw"))
            return LZ_PDW;
        return LZ_XML;
    }
    return LZ_UNKNOWN;
}
