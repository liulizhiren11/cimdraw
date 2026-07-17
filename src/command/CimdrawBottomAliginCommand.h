#ifndef CIMDRAWBOTTOMALIGINCOMMAND_H
#define CIMDRAWBOTTOMALIGINCOMMAND_H

#include <QList>
#include <QScopedPointer>
#include <QUndoCommand>

class QGraphicsItem;
class CimdrawScene;
class CimdrawBottomAlignCommandPrivate;
class CimdrawBottomAlignCommand : public QUndoCommand
{
public:
    CimdrawBottomAlignCommand(QList<QGraphicsItem*>items,CimdrawScene* scene, QUndoCommand* parent = nullptr);
    
    bool validateChange();

    void undo();

    void redo();

private:
    Q_DECLARE_PRIVATE(CimdrawBottomAlignCommand);
    QScopedPointer<CimdrawBottomAlignCommandPrivate> d_ptr;
};
#endif
