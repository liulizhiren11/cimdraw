#ifndef LZCONTROLCOMMAND_H
#define LZCONTROLCOMMAND_H

#include <QUndoCommand>
#include <QVariant>
#include <QScopedPointer>

class LzControlCommandPrivate;

class LzControlCommand : public QUndoCommand
{
public:
    LzControlCommand(QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;
private:
    Q_DECLARE_PRIVATE(LzControlCommand);
    QScopedPointer<LzControlCommandPrivate> d_ptr;
};
#endif;
