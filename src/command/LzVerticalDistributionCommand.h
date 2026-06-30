#ifndef LZVERTICALDISTRIBUTIONCOMMAND_H
#define LZVERTICALDISTRIBUTIONCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class LzScene;
class LzVerticalDistributionCommandPrivate;
class LzVerticalDistributionCommand : public QUndoCommand
{
public:
    LzVerticalDistributionCommand(QList<QGraphicsItem*> items,LzScene* scene,QUndoCommand* command = nullptr);

    bool validateChange();
    
    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(LzVerticalDistributionCommand);
    QScopedPointer<LzVerticalDistributionCommandPrivate> d_ptr;
};
#endif
