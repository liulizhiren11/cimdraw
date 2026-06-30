#ifndef LZLEFTALIGNCOMMAND_H
#define LZLEFTALIGNCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>
#include <QObject>

class QGraphicsItem;
class LzScene;
class LzLeftAlignCommandPrivate;
class LzLeftAlignCommand : public QUndoCommand
{
public:
    LzLeftAlignCommand(QList<QGraphicsItem*> items, LzScene* scene, QUndoCommand* command = nullptr);

    bool validateChange();

    void undo();

    void redo();
private:
    Q_DECLARE_PRIVATE(LzLeftAlignCommand);
    QScopedPointer<LzLeftAlignCommandPrivate> d_ptr;
};

#endif
