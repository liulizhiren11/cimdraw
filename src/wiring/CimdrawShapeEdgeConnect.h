#ifndef CIMDRAWSHAPEEDGECONNECT_H
#define CIMDRAWSHAPEEDGECONNECT_H

#include <QPointF>

#include <QGraphicsItem>

template <typename BaseType>
class TmpBase;

class CimdrawConnectPoint;
class CimdrawConnectLine;

/** 将场景坐标吸附到图元矩形周长最近点（支持旋转/缩放图元） */
bool cimdrawSnapTmpShapeSceneToRectEdge(const TmpBase<QGraphicsItem>* shape, const QPointF& scenePos,
                                   qreal maxDist, QPointF& anchorScene);

/** 在周长位置创建或复用动态连接点（dir > LEFT_DIRECTION 的为可清理动态点） */
CimdrawConnectPoint* cimdrawEnsureTmpShapeConnectPointAtScene(TmpBase<QGraphicsItem>* shape,
                                                    const QPointF& scenePos);

void cimdrawRemoveTmpShapeDynamicConnectPointIfUnused(TmpBase<QGraphicsItem>* shape, CimdrawConnectPoint* port);

/** 移除 pointStruct 中已销毁/脱离场景的悬空指针 */
void cimdrawPruneStaleConnectPointsOnShape(TmpBase<QGraphicsItem>* shape);

/** 删除连接线后清理母线/图元上未使用的动态端口 */
void cimdrawCleanupAfterConnectLineRemoved(CimdrawConnectLine* line);

#endif
