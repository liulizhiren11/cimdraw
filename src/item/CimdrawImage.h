#ifndef CIMDRAWIMAGE_H
#define CIMDRAWIMAGE_H

#include "CimdrawItem.h"

class QPainterPath;
class CimdrawImagePrivate;

class CimdrawImage : public CimdrawItem
{
    Q_OBJECT
public:
    explicit CimdrawImage(QGraphicsItem* parent = nullptr);
    CimdrawImage(const QRectF& pos,QGraphicsItem* parent = nullptr);
    CimdrawImage(const QString& subfile, const QRectF& pos, QGraphicsItem* parent = nullptr);

    QPainterPath shape() const;

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    void updateCoordinate();

    bool saveXml(QDomElement *g);

    bool loadXml(QDomElement *g);

    QString className() const;

    QString shapeName() const;

    void loadImage();

    void loadImage(QPixmap* pixmap, const QString& filename);
protected:
    Q_DECLARE_PRIVATE(CimdrawImage);
    QScopedPointer<CimdrawImagePrivate> d_ptr;
};

class CimdrawImagePrivate
{
    Q_DECLARE_PUBLIC(CimdrawImage)
public:
    CimdrawImagePrivate(CimdrawImage* arc)
        :q_ptr(arc)
    {

    }
    QPixmap pixmap;
    QString subFile;
    CimdrawImage* q_ptr;
};
#endif
