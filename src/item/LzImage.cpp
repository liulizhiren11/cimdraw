#include "LzImage.h"
#include "LzObjectFactory.h"
#include "LzViewConfig.h"
#include <QPainter>
#include <QApplication>
#include <QFileInfo>

LzImage::LzImage(QGraphicsItem* parent)
    :LzItem(parent),d_ptr(new LzImagePrivate(this))
{
    Q_D(LzImage);
    setUseFont(false);
}

LzImage::LzImage(const QRectF& pos,QGraphicsItem* parent)
    :LzItem(parent),d_ptr(new LzImagePrivate(this))
{
    Q_D(LzImage);
    setUseFont(false);

    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0,0, itemWidth,itemHeight);
    itemInitialRect = itemPosition;
    updateCoordinate();
}

LzImage::LzImage(const QString& subfile, const QRectF& pos, QGraphicsItem* parent)
    :LzItem(parent),d_ptr(new LzImagePrivate(this))
{
    Q_D(LzImage);
    setUseFont(false);

    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0,0, itemWidth,itemHeight);
    itemInitialRect = itemPosition;

    d->subFile = subfile;
    loadImage();
    updateCoordinate();
}

QPainterPath LzImage::shape() const
{
    QPainterPath path;
    path.addRect(itemPosition);
    return path;
}

QRectF LzImage::boundingRect() const
{
    return itemPosition;
}

void LzImage::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_D(LzImage);
    prepare(painter);
    painter->drawPixmap(itemPosition.toRect(), d->pixmap);
}

void LzImage::updateCoordinate()
{
    QPointF pt1, pt2, delta;
    pt1 = mapToScene(transformOriginPoint());
    pt2 = mapToScene(itemPosition.center());
    delta = pt1 - pt2;
    prepareGeometryChange();
    if (!parentItem())
    {
        itemPosition = QRectF(-itemWidth / 2, -itemHeight / 2, itemWidth, itemHeight);
        itemWidth = itemPosition.width();
        itemHeight = itemPosition.height();
        setTransform(transform().translate(delta.x(), delta.y()));
        setTransformOriginPoint(itemPosition.center());
        moveBy(-delta.x(), -delta.y());
        setTransform(transform().translate(-delta.x(), -delta.y()));
        updateHandles();
    }
    itemInitialRect = itemPosition;
}

bool LzImage::saveXml(QDomElement *g)
{
    Q_D(const LzImage);
    g->setAttribute("shape", LZ_IMAGE);
    g->setAttribute("subfile", d->subFile);
    LzItem::saveXml(g);
    return true;
}

bool LzImage::loadXml(QDomElement *g)
{
    Q_D(LzImage);
    LzItem::loadXml(g);
    QString attr = g->attribute("subfile");
    if (!attr.isEmpty())
    {
        if (attr.contains("/") || attr.contains("\\"))
        {
            d->subFile = QFileInfo(attr).fileName();
        }
        else
        {
            d->subFile = attr;
        }
        loadImage();
    }
    updateCoordinate();
    return true;
}

QString LzImage::className() const
{
    return "LzImage";
}

QString LzImage::shapeName() const
{
    return "image";
}

void LzImage::loadImage()
{
    Q_D(LzImage);
    loadImage(&d->pixmap, d->subFile);
}

void LzImage::loadImage(QPixmap* pixmap, const QString& filename)
{
    QString filepath = filename;
    if (!filename.contains('\\') && !filename.contains('/'))
    {
        filepath = QString("%1/../../%2/%3").arg(QApplication::applicationDirPath()).arg(FILE_IMAGE).arg(filename);
    }

    pixmap->load(filepath);
}

REGISTER_OBJECT_CREATOR(LzImage, image)
