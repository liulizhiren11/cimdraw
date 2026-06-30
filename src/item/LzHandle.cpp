#include "LzHandle.h"
#include "LzConnectLine.h"
#include <QColor>
#include <QGraphicsSceneContextMenuEvent>
#include <QPainter>

class LzHandlePrivate
{
    Q_DECLARE_PUBLIC(LzHandle)
public:
    LzHandlePrivate(LzHandle* handle,int d,bool ret)
        :q_ptr(handle),dir(d),controlPoint(ret),state(HANDLE_OFF)
    {

    }
    LzHandle* q_ptr;
    HANDLE_STATE state;
    bool controlPoint;
    QColor borderColor;
    int dir;
};

LzHandle::LzHandle(QGraphicsItem* parent, int d,bool control)
    :QGraphicsRectItem(-5,-5,10,10,parent),
      d_ptr(new LzHandlePrivate(this,d,control))
{
    this->setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::NoButton);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    hide();
};

LzHandle::~LzHandle()
{

}

int LzHandle::dir() const
{
    return d_ptr->dir;
}

void LzHandle::move(qreal x, qreal y)
{
    setPos(x, y);
}

void LzHandle::setState(HANDLE_STATE state)
{
    if (d_ptr->state == state)
    {
        return;
    }
    switch (state)
    {
    case HANDLE_OFF:
        hide();
        break;
    case HANDLE_INACTIVE:
    case HANDLE_ACTIVE:
        show();
        break;
    }
    d_ptr->borderColor = Qt::white;
    d_ptr->state = state;
}

void LzHandle::setDir(int dir)
{
    d_ptr->dir = dir;
}

void LzHandle::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsRectItem::hoverLeaveEvent(event);
}

void LzHandle::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsRectItem::hoverLeaveEvent(event);
}

void LzHandle::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QGraphicsItem* p = parentItem();
    if (LzConnectLine* line = qgraphicsitem_cast<LzConnectLine*>(p))
    {
        event->accept();
        line->showContextMenu(event);
        return;
    }
    QGraphicsRectItem::contextMenuEvent(event);
}

void LzHandle::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setPen(Qt::SolidLine);
    painter->setBrush(QBrush(d_ptr->borderColor));

    painter->setRenderHint(QPainter::Antialiasing, false);

    if (d_ptr->controlPoint)
    {
        painter->setPen(QPen(Qt::red, Qt::SolidLine));
        painter->setBrush(Qt::blue);
        painter->drawEllipse(rect().center(),rect().width()/2,rect().height()/2);
    }
    else
    {
        painter->drawEllipse(rect().center(),rect().width()/2,rect().height()/2);
    }
}
