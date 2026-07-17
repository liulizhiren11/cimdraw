#ifndef CIMDRAWROTATECOMMAND_H
#define CIMDRAWROTATECOMMAND_H

#include <QGraphicsItem>
#include <QList>
#include <QScopedPointer>
#include <QUndoCommand>

class CimdrawRotateCommandPrivate;
class CimdrawScene;

class CimdrawRotateCommand : public QUndoCommand
{
public:
    CimdrawRotateCommand(const QList<QGraphicsItem*>& items,
                    CimdrawScene* scene,
                    qreal angleDegrees,
                    QUndoCommand* parent = nullptr);
    ~CimdrawRotateCommand() override;

    bool validateChange();
    void undo() override;
    void redo() override;

private:
    Q_DECLARE_PRIVATE(CimdrawRotateCommand)
    QScopedPointer<CimdrawRotateCommandPrivate> d_ptr;
};

#endif
