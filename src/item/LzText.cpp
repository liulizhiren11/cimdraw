#include "LzText.h"
#include "LzObjectFactory.h"
#include "LzToolboxIconPaint.h"
#include "LzAttributeManager.h"
#include <QPainter>
#include <QGraphicsTextItem>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextOption>

class LzTextPrivate
{
    Q_DECLARE_PUBLIC(LzText)
public:
    LzTextPrivate(LzText* text)
        :q_ptr(text)
    {
        doc.setDocumentMargin(0);
    }
    QString text;
    QTextDocument doc;
    QRectF textRect;
    LzText* q_ptr;
};

LzText::LzText(QGraphicsItem* parent)
    :LzItem(parent),d_ptr(new LzTextPrivate(this))
{

}

LzText::LzText(const QRectF& pos,QGraphicsItem* parent)
    :LzItem(parent),d_ptr(new LzTextPrivate(this))
{
    d_ptr->text = "Text";

    QFontMetrics fontMetrics(itemFont);
    d_ptr->textRect = fontMetrics.boundingRect(d_ptr->text);

    itemWidth = d_ptr->textRect.width() * 2;
    itemHeight = d_ptr->textRect.height() * 2;

    itemPosition = QRectF(0, 0, itemWidth, itemHeight);
    itemInitialRect = itemPosition;

    updateCoordinate();
    updatePoints();
}

QVariant LzText::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
    {
        QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(parentItem());
        if (!g)
        {
            setState(value.toBool() ? HANDLE_ACTIVE : HANDLE_OFF);
        }
        else
        {
            setSelected(false);
            return QVariant::fromValue<bool>(false);
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

QRectF LzText::boundingRect() const
{
    return itemPosition;
}

void LzText::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    prepare(painter);
    // 设置文档大小为项目大小
    d_ptr->doc.setTextWidth(itemPosition.width());

    // 设置对齐方式
    QTextOption opt;
    opt.setAlignment(Qt::AlignCenter);
    d_ptr->doc.setDefaultTextOption(opt);

    d_ptr->doc.setPlainText(d_ptr->text);

    painter->translate(itemPosition.center().x()-itemWidth/2,itemPosition.center().y()-itemHeight/4);
    d_ptr->doc.drawContents(painter);
}

void LzText::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 2 || rect.height() < 2)
        return;
    lzApplyItemDefaultPenBrush(painter);
    QTextDocument doc;
    doc.setDocumentMargin(0);
    doc.setPlainText(QStringLiteral("Text"));
    doc.setTextWidth(rect.width());
    QTextOption opt;
    opt.setAlignment(Qt::AlignCenter);
    doc.setDefaultTextOption(opt);
    if (LzAttributeManager::useFont_)
        doc.setDefaultFont(LzAttributeManager::fontStyle_);
    painter->save();
    painter->translate(rect.topLeft());
    doc.drawContents(painter, QRectF(0, 0, rect.width(), rect.height()));
    painter->restore();
}

void LzText::updateCoordinate()
{
    QPointF pt1, pt2, delta;
    pt1 = mapToScene(transformOriginPoint());
    pt2 = mapToScene(itemPosition.center());
    delta = pt1 - pt2;
    prepareGeometryChange();
    if (!parentItem())
    {
        itemPosition = QRectF(-itemWidth / 2, -itemHeight / 2, itemWidth, itemHeight);
        itemWidth = itemPosition.width();
        itemHeight = itemPosition.height();
        setTransform(transform().translate(delta.x(), delta.y()));
        setTransformOriginPoint(itemPosition.center());
        moveBy(-delta.x(), -delta.y());
        setTransform(transform().translate(-delta.x(), -delta.y()));
        updateHandles();
        updatePoints();
    }
    itemInitialRect = itemPosition;
}

bool LzText::saveXml(QDomElement *g)
{
    if (!g)
        return false;
    return LzItem::saveXml(g);
}

bool LzText::loadXml(QDomElement *g)
{
    return LzItem::loadXml(g);
}

void LzText::stretch(int handle, double sx, double sy, const QPointF & origin)
{

}

QGraphicsItem* LzText::duplicate()
{
    return nullptr;
}

QString LzText::className() const
{
    return "LzText";
}

QString LzText::shapeName() const
{
    return "text";
}

REGISTER_OBJECT_CREATOR(LzText, text)
