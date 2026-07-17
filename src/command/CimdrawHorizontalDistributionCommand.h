#ifndef CIMDRAWHORIZONTALDISTRIBUTIONCOMMAND_H
#define CIMDRAWHORIZONTALDISTRIBUTIONCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class CimdrawScene;
class CimdrawHorizontalDistributionCommandPrivate;
class CimdrawHorizontalDistributionCommand : public QUndoCommand
{
public:
    CimdrawHorizontalDistributionCommand(QList<QGraphicsItem*> items,CimdrawScene* scene, QUndoCommand* parent = nullptr);

    bool validateChange();
    
    void undo() override;

    void redo() override;

private:
    Q_DECLARE_PRIVATE(CimdrawHorizontalDistributionCommand);
    QScopedPointer<CimdrawHorizontalDistributionCommandPrivate> d_ptr;
};
#endif
