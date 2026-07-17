#ifndef CIMDRAWSCENESELECTIONSLOTWORKBENCHCONTROLLER_H
#define CIMDRAWSCENESELECTIONSLOTWORKBENCHCONTROLLER_H

#include <QGraphicsItem>
#include <QList>

class CimdrawScene;

class CimdrawSceneSelectionSlotWorkbenchController
{
public:
    enum class QueryItemAction
    {
        FirstSelection
    };

    enum class QueryItemsAction
    {
        Selections
    };

    enum class SelectionAction
    {
        Prune,
        Clear
    };

    enum class ItemSelectionAction
    {
        AddSelection,
        RemoveSelection,
        RemoveSceneItem,
        InsertSelection,
        DeleteSelection
    };

    enum class ItemsSelectionAction
    {
        ReplaceSelection,
        AddSelections,
        RemoveSelections
    };

    QGraphicsItem* firstSelection(CimdrawScene* scene) const;
    QList<QGraphicsItem*> selections(CimdrawScene* scene) const;
    QGraphicsItem* dispatch(CimdrawScene* scene, QueryItemAction action) const;
    QList<QGraphicsItem*> dispatch(CimdrawScene* scene, QueryItemsAction action) const;
    bool dispatch(CimdrawScene* scene, SelectionAction action) const;
    bool dispatch(CimdrawScene* scene, ItemSelectionAction action, QGraphicsItem* item) const;
    int dispatch(CimdrawScene* scene, ItemsSelectionAction action, const QList<QGraphicsItem*>& items) const;

    void pruneSelectionList(CimdrawScene* scene) const;
    bool replaceSelection(CimdrawScene* scene, const QList<QGraphicsItem*>& items) const;
    bool clearSelection(CimdrawScene* scene) const;
    bool addSelection(CimdrawScene* scene, QGraphicsItem* item) const;
    bool removeSelection(CimdrawScene* scene, QGraphicsItem* item) const;

    bool removeSceneItem(CimdrawScene* scene, QGraphicsItem* item) const;
    bool insertSelection(CimdrawScene* scene, QGraphicsItem* item) const;
    bool deleteSelection(CimdrawScene* scene, QGraphicsItem* item) const;
    int addSelections(CimdrawScene* scene, const QList<QGraphicsItem*>& items) const;
    int removeSelections(CimdrawScene* scene, const QList<QGraphicsItem*>& items) const;
};

#endif
