#ifndef LZVALUECHANGEDCOMMAND_H
#define LZVALUECHANGEDCOMMAND_H

#include <QUndoCommand>
#include <QVariant>
#include <QScopedPointer>
#include <QObject>
#include <QProperty>

class LzScene;
class LzValueChangeCommandPrivate;

class LzValueChangeCommand : public QObject , public QUndoCommand
{
    Q_OBJECT
public:
    LzValueChangeCommand(LzScene* scene,QObject* object,int index, QVariant oldValue, QVariant newValue, QUndoCommand* command = nullptr);
    ~LzValueChangeCommand();

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(LzValueChangeCommand);
    QScopedPointer<LzValueChangeCommandPrivate> d_ptr;
};
#endif
