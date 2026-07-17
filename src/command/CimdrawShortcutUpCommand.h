#ifndef CIMDRAWSHORTCUTUPCOMMAND_H
#define CIMDRAWSHORTCUTUPCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class CimdrawScene;
class CimdrawShortcutUpCommandPrivate;
class CimdrawShortcutUpCommand : public QUndoCommand
{
public:
    CimdrawShortcutUpCommand(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF& delta,QUndoCommand* parent = nullptr);

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawShortcutUpCommand);
    QScopedPointer<CimdrawShortcutUpCommandPrivate> d_ptr;
};
#endif