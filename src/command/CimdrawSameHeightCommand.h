#ifndef CIMDRAWSAMEHEIGHTCOMMAND_H
#define CIMDRAWSAMEHEIGHTCOMMAND_H

#include <QUndoCommand>
#include <QList>
#include <QScopedPointer>

class CimdrawScene;
class QGraphicsItem;
class CimdrawSameHeightCommandPrivate;
class CimdrawSameHeightCommand : public QUndoCommand
{
public:
    CimdrawSameHeightCommand(const QList<QGraphicsItem*>& items, CimdrawScene* scene, QUndoCommand* parent = nullptr);

    bool validateChange();
    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawSameHeightCommand);
    QScopedPointer<CimdrawSameHeightCommandPrivate> d_ptr;
};

#endif // CIMDRAWSAMEHEIGHTCOMMAND_H
