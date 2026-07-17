#include <QFileInfo>
#include <QTextStream>
#include <QDomProcessingInstruction>
#include <QFile>
#include <QColor>
#include <QLineF>

#include "CimdrawFile.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "Item/TmpBase.h"
#include "Item/CimdrawItem.h"
#include "Item/CimdrawGroup.h"
#include "item/CimdrawConnectLine.h"
#include "item/CimdrawConnectPoint.h"
#include "item/CimdrawObjectFactory.h"
#include "topology/TopologyBindingUtils.h"

#include <limits>

class CimdrawFilePrivate
{
    Q_DECLARE_PUBLIC(CimdrawFile)
public:
    CimdrawFilePrivate(CimdrawFile* file)
        :q_ptr(file)
    {

    }
    CimdrawFile* q_ptr;
    QString filePath;
};

namespace {
void ensureSavableItemStableIds(const QList<QGraphicsItem*>& items);
CimdrawItem* findNearestBindableItem(CimdrawScene* scene, const QPointF& scenePos, QGraphicsItem* excludeItem = nullptr);
QHash<QString, QGraphicsItem*> buildSceneTopologyBindingIndex(CimdrawScene* scene);
}

CimdrawFile::CimdrawFile(const QString& path)
    :d_ptr(new CimdrawFilePrivate(this))
{
    d_ptr->filePath = path;
}

CimdrawFile::~CimdrawFile()
{

}

bool CimdrawFile::save(CimdrawScene* scene)
{
    SAVE_FILE_SUFFIX type = getFileTypeByName(d_ptr->filePath);
    switch(type)
    {
    case SAVE_FILE_SUFFIX::CIMDRAW_XML:
    case SAVE_FILE_SUFFIX::CIMDRAW_PDW:
    {
        return saveAsXml(scene);
        break;
    }
    default:
        return false;
    }
}

bool CimdrawFile::saveAsXml(CimdrawScene* scene)
{
    QFile f(d_ptr->filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }
    QTextStream ts(&f);
    QDomDocument doc("CimdrawDraw");
    QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);

    QDomElement root = doc.createElement("CimdrawFile");
    doc.appendChild(root);
    QDomElement header = doc.createElement("Header");
    QColor canvasColor = scene->backgroundBrush().color();
    if (CimdrawView* view = scene->getView())
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
        if (auto* li = dynamic_cast<CimdrawItem*>(obj))
            ok = li->saveXml(&oe);
        else if (auto* grp = dynamic_cast<CimdrawGroup*>(obj))
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
        if (auto* grp = dynamic_cast<CimdrawGroup*>(item))
        {
            grp->ensureCimdrawObjectId();
            continue;
        }
        if (auto* li = dynamic_cast<CimdrawItem*>(item))
            li->ensureCimdrawObjectId();
    }
}

CimdrawItem* findNearestBindableItem(CimdrawScene* scene, const QPointF& scenePos, QGraphicsItem* excludeItem)
{
    if (!scene)
        return nullptr;

    CimdrawItem* best = nullptr;
    qreal bestDist = std::numeric_limits<qreal>::max();
    for (QGraphicsItem* item : scene->items())
    {
        auto* shape = dynamic_cast<CimdrawItem*>(item);
        if (!shape || shape == excludeItem)
            continue;

        qreal candidateDist = std::numeric_limits<qreal>::max();
        bool hasPort = false;
        for (QGraphicsItem* child : shape->childItems())
        {
            auto* port = qgraphicsitem_cast<CimdrawConnectPoint*>(child);
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

void clearSavableTopLevelItems(CimdrawScene* scene)
{
    if (!scene)
        return;
    const QList<QGraphicsItem*> all = scene->items();
    QList<QGraphicsItem*> removeList;
    for (QGraphicsItem* gi : all) {
        if (gi->parentItem() != nullptr)
            continue;
        if (dynamic_cast<CimdrawItem*>(gi) != nullptr || dynamic_cast<CimdrawGroup*>(gi) != nullptr)
            removeList.append(gi);
    }
    for (QGraphicsItem* gi : removeList)
        delete gi;
}

QHash<QString, QGraphicsItem*> buildSceneTopologyBindingIndex(CimdrawScene* scene)
{
    QHash<QString, QGraphicsItem*> index;
    if (!scene)
        return index;

    for (QGraphicsItem* item : scene->items())
    {
        auto* shape = dynamic_cast<CimdrawItem*>(item);
        if (!shape)
            continue;
        const QString stableId = cimdrawTopologyBindingIdForItem(shape);
        if (!stableId.isEmpty() && !index.contains(stableId))
            index.insert(stableId, shape);
    }
    return index;
}

void rebindLoadedConnectLines(CimdrawScene* scene)
{
    if (!scene)
        return;

    const QHash<QString, QGraphicsItem*> topologyBindingIndex = buildSceneTopologyBindingIndex(scene);

    for (QGraphicsItem* item : scene->items())
    {
        auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (!line)
            continue;

        const ConnectorPathRoutingMode savedRoutingMode = line->pathRoutingMode();
        const QVector<QPointF> savedPath = line->pathInSceneCoords();
        const QPointF startAnchor = savedPath.isEmpty() ? QPointF() : savedPath.first();
        const QPointF endAnchor = savedPath.size() < 2 ? QPointF() : savedPath.last();

        line->restorePersistedEndpointItems(topologyBindingIndex);

        if (!line->getStartItem() && savedPath.size() >= 2)
            line->setStartItem(findNearestBindableItem(scene, startAnchor, line->getEndItem()));
        if (!line->getEndItem() && savedPath.size() >= 2)
            line->setEndItem(findNearestBindableItem(scene, endAnchor, line->getStartItem()));

        if (!line->getStartItem() || !line->getEndItem())
            continue;

        line->restorePersistedEndpointPorts(savedPath);

        line->registerEndpointAttachments();
        line->restorePathSnapshot(savedPath, savedRoutingMode);
    }
}

} // namespace

bool CimdrawFile::savePdw(CimdrawScene* scene)
{
    return saveAsXml(scene);
}

bool CimdrawFile::loadPdw(const QString& filePath, CimdrawScene* scene, QString* errMsg)
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
    CimdrawFile lf(filePath);
    return lf.loadByXml(&doc, scene);
}

bool CimdrawFile::load(QDomDocument* dom,QString* errMsg)
{
    return true;
}

bool CimdrawFile::loadByXml(QDomDocument* dom, CimdrawScene* scene)
{
    if (!dom || !scene)
    {
        return false;
    }

    QDomElement docElem = dom->documentElement();
    if (docElem.tagName() != QLatin1String("CimdrawFile"))
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
                    if (CimdrawView* view = scene->getView())
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

SAVE_FILE_SUFFIX CimdrawFile::getFileTypeByName(const QString& fileName)
{
    QFileInfo fileInfo(fileName);
    QString suffix = fileInfo.suffix().toLower();

    if (suffix == QLatin1String("xml") || suffix == QLatin1String("pdw"))
    {
        if (suffix == QLatin1String("pdw"))
            return CIMDRAW_PDW;
        return CIMDRAW_XML;
    }
    return CIMDRAW_UNKNOWN;
}
