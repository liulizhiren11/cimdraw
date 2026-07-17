#ifndef CIMDRAWTOOLBOXICONPAINT_H
#define CIMDRAWTOOLBOXICONPAINT_H

#include "CimdrawAttributeManager.h"

#include <QBrush>
#include <QPainter>
#include <QPen>

/** 与 TmpBase::prepare 一致：按 CimdrawAttributeManager 默认属性配置 painter（新建图元效果） */
inline void cimdrawApplyItemDefaultPenBrush(QPainter* painter)
{
    if (!painter)
        return;
    if (!CimdrawAttributeManager::usePen_)
        painter->setPen(Qt::NoPen);
    else
    {
        QPen pen(CimdrawAttributeManager::penColor_, CimdrawAttributeManager::penWidth_,
                 static_cast<Qt::PenStyle>(CimdrawAttributeManager::penStyle_));
        painter->setPen(pen);
    }
    if (!CimdrawAttributeManager::useBrush_)
        painter->setBrush(Qt::NoBrush);
    else
    {
        QBrush br(CimdrawAttributeManager::brushColor_,
                  static_cast<Qt::BrushStyle>(CimdrawAttributeManager::brushStyle_));
        painter->setBrush(br);
    }
    painter->setRenderHint(QPainter::Antialiasing);
}

#endif
