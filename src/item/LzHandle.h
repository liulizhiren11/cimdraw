#ifndef LZHANDLE_H
#define LZHANDLE_H

#include <QGraphicsItem>
#include <QScopedPointer>
#include "LzItemConfig.h"

class LzHandlePrivate;

class LzHandle : public QGraphicsRectItem
{
public:
    /** 必须声明独立 Type；否则 qgraphicsitem_cast<LzHandle*> 会与 QGraphicsItem::Type 相等而误匹配任意图元（MSVC 下崩溃） */
    enum { Type = QGraphicsItem::UserType + 5 };

    LzHandle(QGraphicsItem* parent, int d, bool control = false);
    ~LzHandle() override;

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
    QScopedPointer<LzHandlePrivate> d_ptr;
};

/** 仅对仍挂在场景上的图元读取 type()，避免路径重算删除手柄后悬空指针崩溃 */
inline bool lzIsHandle(const QGraphicsItem* item)
{
    if (!item || !item->scene())
        return false;
    return item->type() == LzHandle::Type;
}

inline LzHandle* lzHandleFromItem(QGraphicsItem* item)
{
    return lzIsHandle(item) ? static_cast<LzHandle*>(item) : nullptr;
}

namespace HandleType
{
    typedef QVector<LzHandle*> Handles;
    struct HandleParameters
    {
        Handles handles;
    };
}
#endif