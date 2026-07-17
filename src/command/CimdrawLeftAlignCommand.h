#ifndef CIMDRAWLEFTALIGNCOMMAND_H
#define CIMDRAWLEFTALIGNCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>
#include <QObject>

class QGraphicsItem;
class CimdrawScene;
class CimdrawLeftAlignCommandPrivate;
class CimdrawLeftAlignCommand : public QUndoCommand
{
public:
    CimdrawLeftAlignCommand(QList<QGraphicsItem*> items, CimdrawScene* scene, QUndoCommand* command = nullptr);

    bool validateChange();

    void undo();

    void redo();
private:
    Q_DECLARE_PRIVATE(CimdrawLeftAlignCommand);
    QScopedPointer<CimdrawLeftAlignCommandPrivate> d_ptr;
};

#endif
