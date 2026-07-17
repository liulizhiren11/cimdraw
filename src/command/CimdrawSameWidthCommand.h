#ifndef CIMDRAWSAMEWIDTHCOMMAND_H
#define CIMDRAWSAMEWIDTHCOMMAND_H

#include <QUndoCommand>
#include <QList>
#include <QScopedPointer>

class CimdrawScene;
class QGraphicsItem;
class CimdrawSameWidthCommandPrivate;
class CimdrawSameWidthCommand : public QUndoCommand
{
public:
    CimdrawSameWidthCommand(const QList<QGraphicsItem*>& items, CimdrawScene* scene, QUndoCommand* parent = nullptr);

    bool validateChange();
    
    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawSameWidthCommand);
    QScopedPointer<CimdrawSameWidthCommandPrivate> d_ptr;
};
#endif
