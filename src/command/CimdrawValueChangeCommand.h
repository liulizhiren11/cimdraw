#ifndef CIMDRAWVALUECHANGEDCOMMAND_H
#define CIMDRAWVALUECHANGEDCOMMAND_H

#include <QUndoCommand>
#include <QVariant>
#include <QScopedPointer>
#include <QObject>
#include <QProperty>

class CimdrawScene;
class CimdrawValueChangeCommandPrivate;

class CimdrawValueChangeCommand : public QObject , public QUndoCommand
{
    Q_OBJECT
public:
    CimdrawValueChangeCommand(CimdrawScene* scene,QObject* object,int index, QVariant oldValue, QVariant newValue, QUndoCommand* command = nullptr);
    ~CimdrawValueChangeCommand();

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawValueChangeCommand);
    QScopedPointer<CimdrawValueChangeCommandPrivate> d_ptr;
};
#endif
