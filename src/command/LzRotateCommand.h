#ifndef LZROTATECOMMAND_H
#define LZROTATECOMMAND_H

#include <QGraphicsItem>
#include <QList>
#include <QScopedPointer>
#include <QUndoCommand>

class LzRotateCommandPrivate;
class LzScene;

class LzRotateCommand : public QUndoCommand
{
public:
    LzRotateCommand(const QList<QGraphicsItem*>& items,
                    LzScene* scene,
                    qreal angleDegrees,
                    QUndoCommand* parent = nullptr);
    ~LzRotateCommand() override;

    bool validateChange();
    void undo() override;
    void redo() override;

private:
    Q_DECLARE_PRIVATE(LzRotateCommand)
    QScopedPointer<LzRotateCommandPrivate> d_ptr;
};

#endif
