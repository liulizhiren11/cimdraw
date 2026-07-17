#ifndef CIMDRAWHANDLE_H
#define CIMDRAWHANDLE_H

#include <QGraphicsItem>
#include <QScopedPointer>
#include "CimdrawItemConfig.h"

class CimdrawHandlePrivate;

class CimdrawHandle : public QGraphicsRectItem
{
public:
    /** 必须声明独立 Type；否则 qgraphicsitem_cast<CimdrawHandle*> 会与 QGraphicsItem::Type 相等而误匹配任意图元（MSVC 下崩溃） */
    enum { Type = QGraphicsItem::UserType + 5 };

    CimdrawHandle(QGraphicsItem* parent, int d, bool control = false);
    ~CimdrawHandle() override;

    int type() const override { return Type; }

    int dir() const;
    void move(qreal x, qreal y);
    void setState(HANDLE_STATE state);
    void setDir(int dir);
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* e) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* e) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget *widget) override;
private:
    QScopedPointer<CimdrawHandlePrivate> d_ptr;
};

/** 仅对仍挂在场景上的图元读取 type()，避免路径重算删除手柄后悬空指针崩溃 */
inline bool cimdrawIsHandle(const QGraphicsItem* item)
{
    if (!item || !item->scene())
        return false;
    return item->type() == CimdrawHandle::Type;
}

inline CimdrawHandle* cimdrawHandleFromItem(QGraphicsItem* item)
{
    return cimdrawIsHandle(item) ? static_cast<CimdrawHandle*>(item) : nullptr;
}

namespace HandleType
{
    typedef QVector<CimdrawHandle*> Handles;
    struct HandleParameters
    {
        Handles handles;
    };
}
#endif