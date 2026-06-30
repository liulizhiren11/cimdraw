#include <QPainter>
#include <QRectF>

#include "Item/LzBraceCallout.h"
#include "Command/LzAddCommand.h"
#include "LzBraceCalloutTool.h"
#include "LzScene.h"
#include "LzView.h"

LzBraceCalloutTool::LzBraceCalloutTool(QObject* parent)
    : LzTool(parent)
{
    name = tr("bracecallout");
    createMode = CREATE_MODE_DRAG;
}

LzBraceCalloutTool::~LzBraceCalloutTool()
{
}

LzDrawTypeId LzBraceCalloutTool::getDrawType() const
{
    return LZ_BRACE_CALLOUT;
}

QGraphicsItem* LzBraceCalloutTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    LzBraceCallout* item = new LzBraceCallout(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon LzBraceCalloutTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    LzBraceCallout::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void LzBraceCalloutTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        LzToolManager::getInstance()->changeTool(LZ_SELECTION);
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
        LzView* view = scene->getView();
        view->setModified(true);
        scene->addSelection(pObj);
        QUndoCommand* command = new LzAddCommand(pObj, scene, list);
        view->getStack()->push(command);
        emit view->editChanged();
    }
}
