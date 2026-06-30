#ifndef LZSHORTCUTLEFTCOMMAND_H
#define LZSHORTCUTLEFTCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class LzScene;
class LzShortcutLeftCommandPrivate;
class LzShortcutLeftCommand : public QUndoCommand
{
public:
    LzShortcutLeftCommand(QList<QGraphicsItem*> items,LzScene* scene, const QPointF& delta,QUndoCommand* parent = nullptr);

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(LzShortcutLeftCommand);
    QScopedPointer<LzShortcutLeftCommandPrivate> d_ptr;
};
#endif