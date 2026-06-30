#ifndef LZELLIPSE_H
#define LZELLIPSE_H

#include "LzItem.h"

class QPainter;
class QPainterPath;

class LzEllipse : public LzItem
{
public:
    explicit LzEllipse(QGraphicsItem* parent = nullptr);
    LzEllipse(const QRectF& pos,QGraphicsItem* parent = nullptr);

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
