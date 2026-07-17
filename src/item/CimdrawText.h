#ifndef CIMDRAWTEXT_H
#define CIMDRAWTEXT_H

#include "CimdrawItem.h"

class CimdrawTextPrivate;
class QPainter;
class CimdrawText : public CimdrawItem
{
    Q_OBJECT
public:
    explicit CimdrawText(QGraphicsItem* parent = nullptr);
    CimdrawText(const QRectF& pos,QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    static void paintToolboxIcon(QPainter* painter, const QRectF& rect);

    void updateCoordinate();

    bool saveXml(QDomElement *g);

    bool loadXml(QDomElement *g);

    void stretch(int handle, double sx, double sy, const QPointF & origin);

    QGraphicsItem* duplicate();

    QString className() const;

    QString shapeName() const;

protected:
     QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

    Q_DECLARE_PRIVATE(CimdrawText);
    QScopedPointer<CimdrawTextPrivate> d_ptr;
};

#endif
