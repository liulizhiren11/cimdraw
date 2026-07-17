#ifndef CIMDRAWSHORTCUTDOWNCOMMAND_H
#define CIMDRAWSHORTCUTDOWNCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class CimdrawScene;
class CimdrawShortcutDownCommandPrivate;
class CimdrawShortcutDownCommand : public QUndoCommand
{
public:
    CimdrawShortcutDownCommand(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF& delta,QUndoCommand* parent = nullptr);

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawShortcutDownCommand);
    QScopedPointer<CimdrawShortcutDownCommandPrivate> d_ptr;
};
#endif