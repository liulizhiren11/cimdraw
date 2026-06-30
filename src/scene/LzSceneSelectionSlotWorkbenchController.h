#ifndef LZSCENESELECTIONSLOTWORKBENCHCONTROLLER_H
#define LZSCENESELECTIONSLOTWORKBENCHCONTROLLER_H

#include <QGraphicsItem>
#include <QList>

class LzScene;

class LzSceneSelectionSlotWorkbenchController
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

    QGraphicsItem* firstSelection(LzScene* scene) const;
    QList<QGraphicsItem*> selections(LzScene* scene) const;
    QGraphicsItem* dispatch(LzScene* scene, QueryItemAction action) const;
    QList<QGraphicsItem*> dispatch(LzScene* scene, QueryItemsAction action) const;
    bool dispatch(LzScene* scene, SelectionAction action) const;
    bool dispatch(LzScene* scene, ItemSelectionAction action, QGraphicsItem* item) const;
    int dispatch(LzScene* scene, ItemsSelectionAction action, const QList<QGraphicsItem*>& items) const;

    void pruneSelectionList(LzScene* scene) const;
    bool replaceSelection(LzScene* scene, const QList<QGraphicsItem*>& items) const;
    bool clearSelection(LzScene* scene) const;
    bool addSelection(LzScene* scene, QGraphicsItem* item) const;
    bool removeSelection(LzScene* scene, QGraphicsItem* item) const;

    bool removeSceneItem(LzScene* scene, QGraphicsItem* item) const;
    bool insertSelection(LzScene* scene, QGraphicsItem* item) const;
    bool deleteSelection(LzScene* scene, QGraphicsItem* item) const;
    int addSelections(LzScene* scene, const QList<QGraphicsItem*>& items) const;
    int removeSelections(LzScene* scene, const QList<QGraphicsItem*>& items) const;
};

#endif
