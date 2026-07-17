#ifndef CIMDRAWROUNDRECT_H
#define CIMDRAWROUNDRECT_H

#include "CimdrawItem.h"

class QPainter;
class QPainterPath;
class CimdrawRoundRectPrivate;

class CimdrawRoundRect : public CimdrawItem
{
    Q_OBJECT
    Q_PROPERTY(double rx READ getRx WRITE setRx)
    Q_PROPERTY(double ry READ getRy WRITE setRy)
public:
    explicit CimdrawRoundRect(QGraphicsItem* parent = nullptr);
    CimdrawRoundRect(const QRectF& pos,QGraphicsItem* parent = nullptr);

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
    Q_DECLARE_PRIVATE(CimdrawRoundRect);
    QScopedPointer<CimdrawRoundRectPrivate> d_ptr;
};

#endif
