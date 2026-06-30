#ifndef LZSHORTCUTUPCOMMAND_H
#define LZSHORTCUTUPCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class LzScene;
class LzShortcutUpCommandPrivate;
class LzShortcutUpCommand : public QUndoCommand
{
public:
    LzShortcutUpCommand(QList<QGraphicsItem*> items,LzScene* scene, const QPointF& delta,QUndoCommand* parent = nullptr);

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(LzShortcutUpCommand);
    QScopedPointer<LzShortcutUpCommandPrivate> d_ptr;
};
#endif