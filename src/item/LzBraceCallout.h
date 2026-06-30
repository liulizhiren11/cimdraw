#ifndef LZBRACECALLOUT_H
#define LZBRACECALLOUT_H

#include "LzItem.h"

class QPainter;
class QPainterPath;

class LzBraceCallout : public LzItem
{
public:
    explicit LzBraceCallout(QGraphicsItem* parent = nullptr);
    LzBraceCallout(const QRectF& pos, QGraphicsItem* parent = nullptr);

    QPainterPath shape() const override;
    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    static void paintToolboxIcon(QPainter* painter, const QRectF& rect);

    void updateCoordinate() override;
    void stretch(int handle, double sx, double sy, const QPointF& origin) override;
    QGraphicsItem* duplicate() override;

    bool saveXml(QDomElement* g) override;
    bool loadXml(QDomElement* g) override;

    QString className() override;
    QString shapeName() const override;

private:
    QPainterPath symbolPath() const;
};

#endif
