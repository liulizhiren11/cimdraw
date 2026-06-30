#ifndef LZSHORTCUTDOWNCOMMAND_H
#define LZSHORTCUTDOWNCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class LzScene;
class LzShortcutDownCommandPrivate;
class LzShortcutDownCommand : public QUndoCommand
{
public:
    LzShortcutDownCommand(QList<QGraphicsItem*> items,LzScene* scene, const QPointF& delta,QUndoCommand* parent = nullptr);

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(LzShortcutDownCommand);
    QScopedPointer<LzShortcutDownCommandPrivate> d_ptr;
};
#endif