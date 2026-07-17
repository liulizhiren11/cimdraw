#ifndef CIMDRAWADDCOMMAND_H
#define CIMDRAWADDCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>
#include <QGraphicsItem>
#include <QObject>

class CimdrawScene;
class CimdrawAddCommandPrivate;
class CimdrawAddCommand : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    CimdrawAddCommand(QGraphicsItem* item,CimdrawScene* scene,QList<QGraphicsItem*> items, QUndoCommand* parent = nullptr);
    
    ~CimdrawAddCommand();

    void undo();

    void redo();

private:
    Q_DECLARE_PRIVATE(CimdrawAddCommand);
    QScopedPointer<CimdrawAddCommandPrivate> d_ptr;
};

#endif
