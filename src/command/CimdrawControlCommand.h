#ifndef CIMDRAWCONTROLCOMMAND_H
#define CIMDRAWCONTROLCOMMAND_H

#include <QUndoCommand>
#include <QVariant>
#include <QScopedPointer>

class CimdrawControlCommandPrivate;

class CimdrawControlCommand : public QUndoCommand
{
public:
    CimdrawControlCommand(QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawControlCommand);
    QScopedPointer<CimdrawControlCommandPrivate> d_ptr;
};
#endif;
