#include <QProperty>
#include <QMetaObject>
#include <QMetaProperty>

#include "LzValueChangeCommand.h"
#include "LzScene.h"
#include "Item/TmpBase.h"

class LzValueChangeCommandPrivate
{
    Q_DECLARE_PUBLIC(LzValueChangeCommand)
public:
    LzValueChangeCommandPrivate(LzValueChangeCommand* command)
        :q_ptr(command)
    {

    }
    QObject* object;
    QVariant oldValue;
    QVariant newValue;
    bool changed;
    int index;
    LzScene* scene;
    LzValueChangeCommand* q_ptr;
};


LzValueChangeCommand::LzValueChangeCommand(LzScene* scene,QObject* object, int index, QVariant oldValue, QVariant newValue, QUndoCommand* command)
    :d_ptr(new LzValueChangeCommandPrivate(this))
{
    Q_D(LzValueChangeCommand);
    d->scene = scene;
    d->object = object;
    d->oldValue = oldValue;
    d->newValue = newValue;
    d->changed = true;
    d->index = index;
}

LzValueChangeCommand::~LzValueChangeCommand()
{

}

//TODO:值变化时触发他的刷新
void LzValueChangeCommand::undo()
{
    Q_D(LzValueChangeCommand);

    const QMetaObject* metaObject = d->object->metaObject();
    QMetaProperty metaProperty = metaObject->property(d->index);
    metaProperty.write(d->object,d->oldValue);
    d->changed = false;
    TmpShape* test = dynamic_cast<TmpShape*>(d->object);
    QList<QGraphicsItem*> list;
    list.append(test);
    emit d_ptr->scene->currentObjectChanged(list);
}

void LzValueChangeCommand::redo()
{
    Q_D(LzValueChangeCommand);
    if(!d->changed)
    {
        const QMetaObject* metaObject = d->object->metaObject();
        QMetaProperty metaProperty = metaObject->property(d->index);
        metaProperty.write(d->object,d->newValue);
        TmpShape* test = dynamic_cast<TmpShape*>(d->object);
        QList<QGraphicsItem*> list;
        list.append(test);
        emit d_ptr->scene->currentObjectChanged(list);
    }
}
