#ifndef LZCONNECTPOINT_H
#define LZCONNECTPOINT_H

#include <QGraphicsItem>
#include <QScopedPointer>
#include "LzItemConfig.h"
#include "LzConnectConfig.h"

class LzConnectPointPrivate;
class LzConnectPoint : public QGraphicsItem
{
public:
    LzConnectPoint(QGraphicsItem* parent);
    ~LzConnectPoint();

    enum { Type = UserType + 1 };

    int type() const
    {
        return Type;
    }
    /**
     * @brief    获取连接点位置
     * @return   int 
     * @date     2025-05-02
     */
    int dir() const;
    /**
     * @brief    设置连接点位置
     * @param    d 
     * @return   void 
     * @date     2025-05-02
     */
    void setDir(int d);

    /**
     * @brief    移动连接点
     * @param    x 
     * @param    y 
     * @return   void 
     * @date     2025-05-02
     */
    void move(qreal x, qreal y);

    /**
     * @brief    设置连接点状态
     * @param    state 
     * @return   void 
     * @date     2025-05-02
     */
    void setState(HANDLE_STATE state);
    CONNECT_DIRECTION getDirection() const;
    
    void setDirection(CONNECT_DIRECTION dir);
    QString portTag() const;
    void setPortTag(const QString& tag);
    /** 电气吸附点相对「锚点」沿法向再外偏的像素（默认 12，便于 draw.io 式留白）；SLD 图元宜设为 0 使折线与符号导体对齐 */
    void setConnectionLeadOut(qreal pixels);
    qreal connectionLeadOut() const;
    //绑定父图元的相对位置
    void bindToItem(QGraphicsItem* target, const QPointF& offset);

    QPointF getRelativePos() const;

    /// 连接点圆心在本图元局部坐标（用于绘制圆的位置）
    QPointF connectionCenterLocal() const;
    /// 连接点圆心在场景坐标，折线起点/终点应与此对齐
    QPointF connectionCenterInScene() const;

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *e) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *e) override;
    QPainterPath shape() const override;
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget *widget);
private:
    QScopedPointer<LzConnectPointPrivate> d_ptr;
};

namespace ConnectPoint
{
    typedef QVector<LzConnectPoint*> Points;
    struct ConnectPoint
    {
        //用于计算连接数量,后续可能用于连接计算连接状态断开等操作
        ConnectCount count;
        Points points;
    };
};

#endif
