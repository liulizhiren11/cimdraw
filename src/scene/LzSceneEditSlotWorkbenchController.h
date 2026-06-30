#ifndef LZSCENEEDITSLOTWORKBENCHCONTROLLER_H
#define LZSCENEEDITSLOTWORKBENCHCONTROLLER_H

#include <QPointF>

class LzScene;

class LzSceneEditSlotWorkbenchController
{
public:
    enum class Action
    {
        AlignLeft,
        AlignRight,
        AlignTop,
        AlignBottom,
        AlignVCenter,
        AlignHCenter,
        AutoCol,
        AutoRow,
        SameWidth,
        SameHeight,
        SameSize,
        Cut,
        Copy,
        Paste,
        Remove,
        LevelUp,
        LevelDown,
        ToTop,
        ToBottom,
        Group,
        Ungroup
    };

    enum class ShortcutDirection
    {
        Left,
        Right,
        Up,
        Down
    };

    bool dispatch(LzScene* scene, Action action) const;
    bool dispatchShortcut(LzScene* scene,
                          ShortcutDirection direction,
                          const QPointF& delta) const;

    bool alignLeft(LzScene* scene) const;
    bool alignRight(LzScene* scene) const;
    bool alignTop(LzScene* scene) const;
    bool alignBottom(LzScene* scene) const;
    bool alignVCenter(LzScene* scene) const;
    bool alignHCenter(LzScene* scene) const;
    bool autoCol(LzScene* scene) const;
    bool autoRow(LzScene* scene) const;
    bool sameWidth(LzScene* scene) const;
    bool sameHeight(LzScene* scene) const;
    bool sameSize(LzScene* scene) const;

    bool shortcutLeft(LzScene* scene, const QPointF& delta) const;
    bool shortcutRight(LzScene* scene, const QPointF& delta) const;
    bool shortcutUp(LzScene* scene, const QPointF& delta) const;
    bool shortcutDown(LzScene* scene, const QPointF& delta) const;

    bool cut(LzScene* scene) const;
    bool copy(LzScene* scene) const;
    bool paste(LzScene* scene) const;
    bool remove(LzScene* scene) const;
    bool levelUp(LzScene* scene) const;
    bool levelDown(LzScene* scene) const;
    bool toTop(LzScene* scene) const;
    bool toBottom(LzScene* scene) const;
    bool rotate(LzScene* scene, qreal angleDegrees) const;
    bool group(LzScene* scene) const;
    bool ungroup(LzScene* scene) const;
};

#endif
