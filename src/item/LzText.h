#ifndef LZTEXT_H
#define LZTEXT_H

#include "LzItem.h"

class LzTextPrivate;
class QPainter;
class LzText : public LzItem
{
    Q_OBJECT
public:
    explicit LzText(QGraphicsItem* parent = nullptr);
    LzText(const QRectF& pos,QGraphicsItem* parent = nullptr);

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

    Q_DECLARE_PRIVATE(LzText);
    QScopedPointer<LzTextPrivate> d_ptr;
};

#endif
