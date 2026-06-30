#ifndef LZHTYPERLINK_H
#define LZHTYPERLINK_H

#include "LzItemConfig.h"
#include "LzItem.h"

class QPainterPath;
class LzHyperLinkPrivate;

class LzHyperLink : public LzItem
{
    Q_OBJECT
public:
    explicit LzHyperLink(QGraphicsItem* parent = nullptr);
    LzHyperLink(const QRectF& pos,QGraphicsItem* parent = nullptr);

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
    Q_DECLARE_PRIVATE(LzHyperLink);
    QScopedPointer<LzHyperLinkPrivate> d_ptr;
};

#endif
