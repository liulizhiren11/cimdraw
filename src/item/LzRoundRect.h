#ifndef LZROUNDRECT_H
#define LZROUNDRECT_H

#include "LzItem.h"

class QPainter;
class QPainterPath;
class LzRoundRectPrivate;

class LzRoundRect : public LzItem
{
    Q_OBJECT
    Q_PROPERTY(double rx READ getRx WRITE setRx)
    Q_PROPERTY(double ry READ getRy WRITE setRy)
public:
    explicit LzRoundRect(QGraphicsItem* parent = nullptr);
    LzRoundRect(const QRectF& pos,QGraphicsItem* parent = nullptr);

    QPainterPath shape() const;

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    static void paintToolboxIcon(QPainter* painter, const QRectF& rect);

    void updateCoordinate();

    bool saveXml(QDomElement *g);

    bool loadXml(QDomElement *g);

    QString className() const;

    QString shapeName() const;

    void setRx(const double& data);

    const double getRx();

    void setRy(const double& data);

    const double getRy();

protected:
    Q_DECLARE_PRIVATE(LzRoundRect);
    QScopedPointer<LzRoundRectPrivate> d_ptr;
};

#endif
