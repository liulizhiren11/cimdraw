#ifndef CIMDRAWELLIPSE_H
#define CIMDRAWELLIPSE_H

#include "CimdrawItem.h"

class QPainter;
class QPainterPath;

class CimdrawEllipse : public CimdrawItem
{
public:
    explicit CimdrawEllipse(QGraphicsItem* parent = nullptr);
    CimdrawEllipse(const QRectF& pos,QGraphicsItem* parent = nullptr);

    QPainterPath shape() const;

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    static void paintToolboxIcon(QPainter* painter, const QRectF& rect);

    void updateCoordinate();

    bool saveXml(QDomElement *g);

    bool loadXml(QDomElement *g);

    QString className() const;

    QString shapeName() const;
protected:

};

#endif
