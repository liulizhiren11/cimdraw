#ifndef LZTOOLBOXICONPAINT_H
#define LZTOOLBOXICONPAINT_H

#include "LzAttributeManager.h"

#include <QBrush>
#include <QPainter>
#include <QPen>

/** 与 TmpBase::prepare 一致：按 LzAttributeManager 默认属性配置 painter（新建图元效果） */
inline void lzApplyItemDefaultPenBrush(QPainter* painter)
{
    if (!painter)
        return;
    if (!LzAttributeManager::usePen_)
        painter->setPen(Qt::NoPen);
    else
    {
        QPen pen(LzAttributeManager::penColor_, LzAttributeManager::penWidth_,
                 static_cast<Qt::PenStyle>(LzAttributeManager::penStyle_));
        painter->setPen(pen);
    }
    if (!LzAttributeManager::useBrush_)
        painter->setBrush(Qt::NoBrush);
    else
    {
        QBrush br(LzAttributeManager::brushColor_,
                  static_cast<Qt::BrushStyle>(LzAttributeManager::brushStyle_));
        painter->setBrush(br);
    }
    painter->setRenderHint(QPainter::Antialiasing);
}

#endif
