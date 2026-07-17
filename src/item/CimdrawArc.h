#ifndef CIMDRAWARC_H
#define CIMDRAWARC_H

#include "CimdrawItem.h"

class QPainter;
class QPainterPath;
class CimdrawArcPrivate;

class CimdrawArc : public CimdrawItem
{
    Q_OBJECT
    Q_PROPERTY(double startAngle READ getStartAngle WRITE setStartAngle)
    Q_PROPERTY(double sweepAngle READ getSweepAngle WRITE setSweepAngle)
    Q_PROPERTY(bool sweepFlag READ getSweepFlag WRITE setSweepFlag)
public:
    explicit CimdrawArc(QGraphicsItem* parent = nullptr);
    CimdrawArc(const QRectF& pos,QGraphicsItem* parent = nullptr);

    QPainterPath shape() const;

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

    const double getStartAngle();
    void setStartAngle(const double& data);

    const double getSweepAngle();
    void setSweepAngle(const double& data);

    const bool getSweepFlag();
    void setSweepFlag(bool data);
protected:
    Q_DECLARE_PRIVATE(CimdrawArc);
    QScopedPointer<CimdrawArcPrivate> d_ptr;
};

#endif
