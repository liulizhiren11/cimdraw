#ifndef CIMDRAWHTYPERLINK_H
#define CIMDRAWHTYPERLINK_H

#include "CimdrawItemConfig.h"
#include "CimdrawItem.h"

class QPainterPath;
class CimdrawHyperLinkPrivate;

class CimdrawHyperLink : public CimdrawItem
{
    Q_OBJECT
public:
    explicit CimdrawHyperLink(QGraphicsItem* parent = nullptr);
    CimdrawHyperLink(const QRectF& pos,QGraphicsItem* parent = nullptr);

    QPainterPath shape() const;

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    void updateCoordinate();

    bool saveXml(QDomElement *g);

    bool loadXml(QDomElement *g);

    QString className() const;

    QString shapeName() const;

    HYPER_LINK_TYPE getLinkType();

    void setLinkeType();


protected:
    Q_DECLARE_PRIVATE(CimdrawHyperLink);
    QScopedPointer<CimdrawHyperLinkPrivate> d_ptr;
};

#endif
