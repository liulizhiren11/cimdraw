#ifndef CIMDRAWWIRINGTOOLUTILS_H
#define CIMDRAWWIRINGTOOLUTILS_H

#include "CimdrawWiringSymbolStyle.h"

class CimdrawScene;
class QGraphicsItem;
class QGraphicsSceneMouseEvent;

/** 将新放置的接线图元压入撤销栈并加入选择（各接线工具共用） */
void cimdrawSldPushAdd(CimdrawScene* scene, QGraphicsItem* pObj, QGraphicsSceneMouseEvent* evt);

#endif // CIMDRAWWIRINGTOOLUTILS_H
