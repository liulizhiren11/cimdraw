#include <QPainter>
#include <QRectF>

#include "Item/CimdrawBraceCallout.h"
#include "Command/CimdrawAddCommand.h"
#include "CimdrawBraceCalloutTool.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"

CimdrawBraceCalloutTool::CimdrawBraceCalloutTool(QObject* parent)
    : CimdrawTool(parent)
{
    name = tr("bracecallout");
    createMode = CREATE_MODE_DRAG;
}

CimdrawBraceCalloutTool::~CimdrawBraceCalloutTool()
{
}

CimdrawDrawTypeId CimdrawBraceCalloutTool::getDrawType() const
{
    return CIMDRAW_BRACE_CALLOUT;
}

QGraphicsItem* CimdrawBraceCalloutTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    CimdrawBraceCallout* item = new CimdrawBraceCallout(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon CimdrawBraceCalloutTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    CimdrawBraceCallout::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void CimdrawBraceCalloutTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
        scene->clearSelection();
        return;
    }

    const QPointF local = evt->scenePos();
    QList<QGraphicsItem*> list = scene->getSelections();
    QRectF rc(local, QSizeF(72, 120));
    QGraphicsItem* pObj = createObject(scene, &rc);
    if (pObj)
    {
        pObj->setPos(local);
        CimdrawView* view = scene->getView();
        view->setModified(true);
        scene->addSelection(pObj);
        QUndoCommand* command = new CimdrawAddCommand(pObj, scene, list);
        view->getStack()->push(command);
        emit view->editChanged();
    }
}
