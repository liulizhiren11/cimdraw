#ifndef CIMDRAWSHORTCUTLEFTCOMMAND_H
#define CIMDRAWSHORTCUTLEFTCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class CimdrawScene;
class CimdrawShortcutLeftCommandPrivate;
class CimdrawShortcutLeftCommand : public QUndoCommand
{
public:
    CimdrawShortcutLeftCommand(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF& delta,QUndoCommand* parent = nullptr);

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawShortcutLeftCommand);
    QScopedPointer<CimdrawShortcutLeftCommandPrivate> d_ptr;
};
#endif