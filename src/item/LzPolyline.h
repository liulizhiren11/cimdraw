#ifndef LZPOLYLINE_H
#define LZPOLYLINE_H

#include "LzItem.h"

class QPainter;
class QPainterPath;
class LzPolylinePrivate;

class LzPolyline : public LzItem
{
public:
    explicit LzPolyline(QGraphicsItem* item = nullptr);

    LzPolyline(const QRectF& pos,QGraphicsItem* parent = nullptr);

    QPainterPath shape() const;

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    static void paintToolboxIcon(QPainter* painter, const QRectF& rect);

    void updateCoordinate() override;

    bool saveXml(QDomElement *g);

    bool loadXml(QDomElement *g);

    QString className() const;

    QString shapeName() const;

    void addPoint(const QPointF& point);

    void endPoint(const QPointF & point);

    void stretch(int handle, double sx, double sy, const QPointF &) override;

    void control(int dir, const QPointF& delta) override;

    void updateHandles() override;

    QPointF opposite(int handle) override;
protected:
    Q_DECLARE_PRIVATE(LzPolyline);
    QScopedPointer<LzPolylinePrivate> d_ptr;
};

#endif
