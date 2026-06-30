#ifndef LZLINE_H
#define LZLINE_H

#include "LzItem.h"

class QPainter;
class QPainterPath;
class LzLinePrivate;

class LzLine : public LzItem
{
    Q_OBJECT
public:
    explicit LzLine(QGraphicsItem* item = nullptr);

    LzLine(const QRectF& pos,QGraphicsItem* parent = nullptr);

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

    int handleCount() const;

    void stretch(int handle, double sx, double sy, const QPointF &) override;

    void control(int dir, const QPointF& delta) override;

    void updateHandles() override;

    QPointF opposite(int handle) override;
protected:
    Q_DECLARE_PRIVATE(LzLine);
    QScopedPointer<LzLinePrivate> d_ptr;
};

#endif
