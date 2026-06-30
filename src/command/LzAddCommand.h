#ifndef LZADDCOMMAND_H
#define LZADDCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>
#include <QGraphicsItem>
#include <QObject>

class LzScene;
class LzAddCommandPrivate;
class LzAddCommand : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    LzAddCommand(QGraphicsItem* item,LzScene* scene,QList<QGraphicsItem*> items, QUndoCommand* parent = nullptr);
    
    ~LzAddCommand();

    void undo();

    void redo();

private:
    Q_DECLARE_PRIVATE(LzAddCommand);
    QScopedPointer<LzAddCommandPrivate> d_ptr;
};

#endif
