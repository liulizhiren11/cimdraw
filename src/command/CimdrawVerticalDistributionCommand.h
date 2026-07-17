#ifndef CIMDRAWVERTICALDISTRIBUTIONCOMMAND_H
#define CIMDRAWVERTICALDISTRIBUTIONCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class CimdrawScene;
class CimdrawVerticalDistributionCommandPrivate;
class CimdrawVerticalDistributionCommand : public QUndoCommand
{
public:
    CimdrawVerticalDistributionCommand(QList<QGraphicsItem*> items,CimdrawScene* scene,QUndoCommand* command = nullptr);

    bool validateChange();
    
    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawVerticalDistributionCommand);
    QScopedPointer<CimdrawVerticalDistributionCommandPrivate> d_ptr;
};
#endif
