#include "CimdrawText.h"
#include "CimdrawObjectFactory.h"
#include "CimdrawToolboxIconPaint.h"
#include "CimdrawAttributeManager.h"
#include <QPainter>
#include <QGraphicsTextItem>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextOption>

class CimdrawTextPrivate
{
    Q_DECLARE_PUBLIC(CimdrawText)
public:
    CimdrawTextPrivate(CimdrawText* text)
        :q_ptr(text)
    {
        doc.setDocumentMargin(0);
    }
    QString text;
    QTextDocument doc;
    QRectF textRect;
    CimdrawText* q_ptr;
};

CimdrawText::CimdrawText(QGraphicsItem* parent)
    :CimdrawItem(parent),d_ptr(new CimdrawTextPrivate(this))
{

}

CimdrawText::CimdrawText(const QRectF& pos,QGraphicsItem* parent)
    :CimdrawItem(parent),d_ptr(new CimdrawTextPrivate(this))
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

QVariant CimdrawText::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
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

QRectF CimdrawText::boundingRect() const
{
    return itemPosition;
}

void CimdrawText::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
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

void CimdrawText::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 2 || rect.height() < 2)
        return;
    cimdrawApplyItemDefaultPenBrush(painter);
    QTextDocument doc;
    doc.setDocumentMargin(0);
    doc.setPlainText(QStringLiteral("Text"));
    doc.setTextWidth(rect.width());
    QTextOption opt;
    opt.setAlignment(Qt::AlignCenter);
    doc.setDefaultTextOption(opt);
    if (CimdrawAttributeManager::useFont_)
        doc.setDefaultFont(CimdrawAttributeManager::fontStyle_);
    painter->save();
    painter->translate(rect.topLeft());
    doc.drawContents(painter, QRectF(0, 0, rect.width(), rect.height()));
    painter->restore();
}

void CimdrawText::updateCoordinate()
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

bool CimdrawText::saveXml(QDomElement *g)
{
    if (!g)
        return false;
    return CimdrawItem::saveXml(g);
}

bool CimdrawText::loadXml(QDomElement *g)
{
    return CimdrawItem::loadXml(g);
}

void CimdrawText::stretch(int handle, double sx, double sy, const QPointF & origin)
{

}

QGraphicsItem* CimdrawText::duplicate()
{
    return nullptr;
}

QString CimdrawText::className() const
{
    return "CimdrawText";
}

QString CimdrawText::shapeName() const
{
    return "text";
}

REGISTER_OBJECT_CREATOR(CimdrawText, text)
