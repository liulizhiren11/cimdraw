#ifndef LZIMAGE_H
#define LZIMAGE_H

#include "LzItem.h"

class QPainterPath;
class LzImagePrivate;

class LzImage : public LzItem
{
    Q_OBJECT
public:
    explicit LzImage(QGraphicsItem* parent = nullptr);
    LzImage(const QRectF& pos,QGraphicsItem* parent = nullptr);
    LzImage(const QString& subfile, const QRectF& pos, QGraphicsItem* parent = nullptr);

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
    Q_DECLARE_PRIVATE(LzImage);
    QScopedPointer<LzImagePrivate> d_ptr;
};

class LzImagePrivate
{
    Q_DECLARE_PUBLIC(LzImage)
public:
    LzImagePrivate(LzImage* arc)
        :q_ptr(arc)
    {

    }
    QPixmap pixmap;
    QString subFile;
    LzImage* q_ptr;
};
#endif
