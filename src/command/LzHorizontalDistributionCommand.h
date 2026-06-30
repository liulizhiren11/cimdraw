#ifndef LZHORIZONTALDISTRIBUTIONCOMMAND_H
#define LZHORIZONTALDISTRIBUTIONCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class LzScene;
class LzHorizontalDistributionCommandPrivate;
class LzHorizontalDistributionCommand : public QUndoCommand
{
public:
    LzHorizontalDistributionCommand(QList<QGraphicsItem*> items,LzScene* scene, QUndoCommand* parent = nullptr);

    bool validateChange();
    
    void undo() override;

    void redo() override;

private:
    Q_DECLARE_PRIVATE(LzHorizontalDistributionCommand);
    QScopedPointer<LzHorizontalDistributionCommandPrivate> d_ptr;
};
#endif
