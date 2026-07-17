#ifndef CIMDRAWSAMESIZECOMMAND_H
#define CIMDRAWSAMESIZECOMMAND_H

#include <QUndoCommand>
#include <QList>
#include <QScopedPointer>

class CimdrawScene;
class QGraphicsItem;
class CimdrawSameSizeCommandPrivate;
class CimdrawSameSizeCommand : public QUndoCommand
{
public:
    CimdrawSameSizeCommand(const QList<QGraphicsItem*>& items, CimdrawScene* scene, QUndoCommand* parent = nullptr);

    bool validateChange();

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawSameSizeCommand);
    QScopedPointer<CimdrawSameSizeCommandPrivate> d_ptr;
};

#endif
