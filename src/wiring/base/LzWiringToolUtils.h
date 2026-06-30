#ifndef LZWIRINGTOOLUTILS_H
#define LZWIRINGTOOLUTILS_H

#include "LzWiringSymbolStyle.h"

class LzScene;
class QGraphicsItem;
class QGraphicsSceneMouseEvent;

/** 将新放置的接线图元压入撤销栈并加入选择（各接线工具共用） */
void lzSldPushAdd(LzScene* scene, QGraphicsItem* pObj, QGraphicsSceneMouseEvent* evt);

#endif // LZWIRINGTOOLUTILS_H
