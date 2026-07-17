#include "CimdrawImage.h"
#include "CimdrawObjectFactory.h"
#include "CimdrawViewConfig.h"
#include <QPainter>
#include <QApplication>
#include <QFileInfo>

CimdrawImage::CimdrawImage(QGraphicsItem* parent)
    :CimdrawItem(parent),d_ptr(new CimdrawImagePrivate(this))
{
    Q_D(CimdrawImage);
    setUseFont(false);
}

CimdrawImage::CimdrawImage(const QRectF& pos,QGraphicsItem* parent)
    :CimdrawItem(parent),d_ptr(new CimdrawImagePrivate(this))
{
    Q_D(CimdrawImage);
    setUseFont(false);

    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0,0, itemWidth,itemHeight);
    itemInitialRect = itemPosition;
    updateCoordinate();
}

CimdrawImage::CimdrawImage(const QString& subfile, const QRectF& pos, QGraphicsItem* parent)
    :CimdrawItem(parent),d_ptr(new CimdrawImagePrivate(this))
{
    Q_D(CimdrawImage);
    setUseFont(false);

    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0,0, itemWidth,itemHeight);
    itemInitialRect = itemPosition;

    d->subFile = subfile;
    loadImage();
    updateCoordinate();
}

QPainterPath CimdrawImage::shape() const
{
    QPainterPath path;
    path.addRect(itemPosition);
    return path;
}

QRectF CimdrawImage::boundingRect() const
{
    return itemPosition;
}

void CimdrawImage::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_D(CimdrawImage);
    prepare(painter);
    painter->drawPixmap(itemPosition.toRect(), d->pixmap);
}

void CimdrawImage::updateCoordinate()
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

bool CimdrawImage::saveXml(QDomElement *g)
{
    Q_D(const CimdrawImage);
    g->setAttribute("shape", CIMDRAW_IMAGE);
    g->setAttribute("subfile", d->subFile);
    CimdrawItem::saveXml(g);
    return true;
}

bool CimdrawImage::loadXml(QDomElement *g)
{
    Q_D(CimdrawImage);
    CimdrawItem::loadXml(g);
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

QString CimdrawImage::className() const
{
    return "CimdrawImage";
}

QString CimdrawImage::shapeName() const
{
    return "image";
}

void CimdrawImage::loadImage()
{
    Q_D(CimdrawImage);
    loadImage(&d->pixmap, d->subFile);
}

void CimdrawImage::loadImage(QPixmap* pixmap, const QString& filename)
{
    QString filepath = filename;
    if (!filename.contains('\\') && !filename.contains('/'))
    {
        filepath = QString("%1/../../%2/%3").arg(QApplication::applicationDirPath()).arg(FILE_IMAGE).arg(filename);
    }

    pixmap->load(filepath);
}

REGISTER_OBJECT_CREATOR(CimdrawImage, image)
