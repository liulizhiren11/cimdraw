#include <QProperty>
#include <QMetaObject>
#include <QMetaProperty>

#include "CimdrawValueChangeCommand.h"
#include "CimdrawScene.h"
#include "Item/TmpBase.h"

class CimdrawValueChangeCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawValueChangeCommand)
public:
    CimdrawValueChangeCommandPrivate(CimdrawValueChangeCommand* command)
        :q_ptr(command)
    {

    }
    QObject* object;
    QVariant oldValue;
    QVariant newValue;
    bool changed;
    int index;
    CimdrawScene* scene;
    CimdrawValueChangeCommand* q_ptr;
};


CimdrawValueChangeCommand::CimdrawValueChangeCommand(CimdrawScene* scene,QObject* object, int index, QVariant oldValue, QVariant newValue, QUndoCommand* command)
    :d_ptr(new CimdrawValueChangeCommandPrivate(this))
{
    Q_D(CimdrawValueChangeCommand);
    d->scene = scene;
    d->object = object;
    d->oldValue = oldValue;
    d->newValue = newValue;
    d->changed = true;
    d->index = index;
}

CimdrawValueChangeCommand::~CimdrawValueChangeCommand()
{

}

//TODO:值变化时触发他的刷新
void CimdrawValueChangeCommand::undo()
{
    Q_D(CimdrawValueChangeCommand);

    const QMetaObject* metaObject = d->object->metaObject();
    QMetaProperty metaProperty = metaObject->property(d->index);
    metaProperty.write(d->object,d->oldValue);
    d->changed = false;
    TmpShape* test = dynamic_cast<TmpShape*>(d->object);
    QList<QGraphicsItem*> list;
    list.append(test);
    emit d_ptr->scene->currentObjectChanged(list);
}

void CimdrawValueChangeCommand::redo()
{
    Q_D(CimdrawValueChangeCommand);
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
