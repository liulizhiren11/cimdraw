#ifndef CIMDRAWSHORTCUTRIGHTCOMMAND_H
#define CIMDRAWSHORTCUTRIGHTCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class CimdrawScene;
class CimdrawShortcutRightCommandPrivate;
class CimdrawShortcutRightCommand : public QUndoCommand
{
public:
    CimdrawShortcutRightCommand(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF& delta,QUndoCommand* parent = nullptr);

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawShortcutRightCommand);
    QScopedPointer<CimdrawShortcutRightCommandPrivate> d_ptr;
};
#endif