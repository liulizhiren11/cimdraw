#ifndef LZSHORTCUTRIGHTCOMMAND_H
#define LZSHORTCUTRIGHTCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class LzScene;
class LzShortcutRightCommandPrivate;
class LzShortcutRightCommand : public QUndoCommand
{
public:
    LzShortcutRightCommand(QList<QGraphicsItem*> items,LzScene* scene, const QPointF& delta,QUndoCommand* parent = nullptr);

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(LzShortcutRightCommand);
    QScopedPointer<LzShortcutRightCommandPrivate> d_ptr;
};
#endif