#ifndef CIMDRAWRECT_H
#define CIMDRAWRECT_H

#include "CimdrawItem.h"

class QGraphicsSceneMouseEvent;
class QPainter;
class QPainterPath;

class CimdrawRect : public CimdrawItem
{
    Q_OBJECT
public:
    explicit CimdrawRect(QGraphicsItem* parent = nullptr);
    CimdrawRect(const QRectF& pos,QGraphicsItem* parent = nullptr);

    QPainterPath shape() const;

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    static void paintToolboxIcon(QPainter* painter, const QRectF& rect);

    void updateCoordinate();

    bool saveXml(QDomElement* g);

    bool loadXml(QDomElement* g);

	void stretch(int handle, double sx, double sy, const QPointF& origin);

    QGraphicsItem* duplicate();

    QString className() const;

	QString shapeName() const;
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
};

#endif