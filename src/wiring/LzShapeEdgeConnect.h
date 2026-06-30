#ifndef LZSHAPEEDGECONNECT_H
#define LZSHAPEEDGECONNECT_H

#include <QPointF>

#include <QGraphicsItem>

template <typename BaseType>
class TmpBase;

class LzConnectPoint;
class LzConnectLine;

/** 将场景坐标吸附到图元矩形周长最近点（支持旋转/缩放图元） */
bool lzSnapTmpShapeSceneToRectEdge(const TmpBase<QGraphicsItem>* shape, const QPointF& scenePos,
                                   qreal maxDist, QPointF& anchorScene);

/** 在周长位置创建或复用动态连接点（dir > LEFT_DIRECTION 的为可清理动态点） */
LzConnectPoint* lzEnsureTmpShapeConnectPointAtScene(TmpBase<QGraphicsItem>* shape,
                                                    const QPointF& scenePos);

void lzRemoveTmpShapeDynamicConnectPointIfUnused(TmpBase<QGraphicsItem>* shape, LzConnectPoint* port);

/** 移除 pointStruct 中已销毁/脱离场景的悬空指针 */
void lzPruneStaleConnectPointsOnShape(TmpBase<QGraphicsItem>* shape);

/** 删除连接线后清理母线/图元上未使用的动态端口 */
void lzCleanupAfterConnectLineRemoved(LzConnectLine* line);

#endif
