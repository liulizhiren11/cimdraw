#ifndef LZSCENEEDITWORKBENCHCONTROLLER_H
#define LZSCENEEDITWORKBENCHCONTROLLER_H

#include <QGraphicsItem>
#include <QList>
#include <QPointF>

class QGraphicsScene;
class QUndoStack;
class LzScene;

class LzSceneEditWorkbenchController
{
public:
    void alignLeft(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;
    void alignRight(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;
    void alignTop(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;
    void alignBottom(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;
    void alignVCenter(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;
    void alignHCenter(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;
    void autoCol(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;
    void autoRow(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;
    void sameWidth(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;
    void sameHeight(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;
    void sameSize(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;

    void shortcutLeft(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack, const QPointF& delta) const;
    void shortcutRight(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack, const QPointF& delta) const;
    void shortcutUp(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack, const QPointF& delta) const;
    void shortcutDown(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack, const QPointF& delta) const;

    void cut(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;
    void copy(const QList<QGraphicsItem*>& selection) const;
    void paste(LzScene* scene, QUndoStack* stack) const;
    void remove(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;

    void levelUp(const QList<QGraphicsItem*>& selection, LzScene* scene, const QGraphicsScene* graphicsScene) const;
    void levelDown(const QList<QGraphicsItem*>& selection, LzScene* scene, const QGraphicsScene* graphicsScene) const;
    void toTop(const QList<QGraphicsItem*>& selection, LzScene* scene, const QGraphicsScene* graphicsScene) const;
    void toBottom(const QList<QGraphicsItem*>& selection, LzScene* scene, const QGraphicsScene* graphicsScene) const;
    void rotate(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack, qreal angleDegrees) const;
    void group(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;
    void ungroup(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const;

    QUndoStack* stackFromScene(LzScene* scene) const;
};

#endif
