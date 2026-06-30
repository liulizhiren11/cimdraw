#ifndef LZITEM_H
#define LZITEM_H

#include "TmpBase.h"
#include <QGraphicsItem>

class LzItem : public QObject, public TmpBase<QGraphicsItem>
{
    Q_OBJECT
    // 定义新属性的 Q_PROPERTY
    Q_PROPERTY(bool usePen READ getUsePen WRITE setUsePen)
    Q_PROPERTY(QColor itemPenColor READ getItemPenColor WRITE setItemPenColor)
    Q_PROPERTY(int itemPenWidth READ getItemPenWidth WRITE setItemPenWidth)
    Q_PROPERTY(int itemPenStyle READ getItemPenStyle WRITE setItemPenStyle)

    Q_PROPERTY(bool useBrush READ getUseBrush WRITE setUseBrush)
    Q_PROPERTY(QColor itemBrushColor READ getItemBrushColor WRITE setItemBrushColor)
    Q_PROPERTY(int itemBrushStyle READ getItemBrushStyle WRITE setItemBrushStyle)

    Q_PROPERTY(bool useFont READ getUseFont WRITE setUseFont)
    Q_PROPERTY(QFont itemFont READ getItemFont WRITE setItemFont)

    Q_PROPERTY(QPointF position READ pos WRITE setPos)
    Q_PROPERTY(qreal rotate READ getItemDegree WRITE setItemDegree)
    Q_PROPERTY(QPointF sceneTransformOriginPoint READ getSceneTransformOriginPoint)
    Q_PROPERTY(int type READ type)

public:
    enum { Type = UserType + 3 };

    virtual int type() const
    {
        return Type;
    }

    explicit LzItem(QGraphicsItem* parent = nullptr);
    LzItem(const QRectF& pos, QGraphicsItem* parent = nullptr);

    bool saveXml(QDomElement* g) override;

    bool loadXml(QDomElement* g) override;

    qreal itemLeft() override;

    qreal itemTop() override;

    qreal itemBottom() override;

    qreal itemRight() override;

    qreal centerX() override;

    qreal centerY() override;

    virtual void setWidth(qreal width) override;

    virtual void setHeight(qreal height) override;

    virtual void setSize(QSizeF size) override;

    virtual void updateConnect(const QPointF& delta) override;

    /** 图元缩放/端口重绑后，刷新挂接的连线几何 */
    void refreshConnectedLines();

protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
};

#endif
