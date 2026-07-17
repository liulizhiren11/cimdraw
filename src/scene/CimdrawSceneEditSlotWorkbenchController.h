#ifndef CIMDRAWSCENEEDITSLOTWORKBENCHCONTROLLER_H
#define CIMDRAWSCENEEDITSLOTWORKBENCHCONTROLLER_H

#include <QPointF>

class CimdrawScene;

class CimdrawSceneEditSlotWorkbenchController
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

    bool dispatch(CimdrawScene* scene, Action action) const;
    bool dispatchShortcut(CimdrawScene* scene,
                          ShortcutDirection direction,
                          const QPointF& delta) const;

    bool alignLeft(CimdrawScene* scene) const;
    bool alignRight(CimdrawScene* scene) const;
    bool alignTop(CimdrawScene* scene) const;
    bool alignBottom(CimdrawScene* scene) const;
    bool alignVCenter(CimdrawScene* scene) const;
    bool alignHCenter(CimdrawScene* scene) const;
    bool autoCol(CimdrawScene* scene) const;
    bool autoRow(CimdrawScene* scene) const;
    bool sameWidth(CimdrawScene* scene) const;
    bool sameHeight(CimdrawScene* scene) const;
    bool sameSize(CimdrawScene* scene) const;

    bool shortcutLeft(CimdrawScene* scene, const QPointF& delta) const;
    bool shortcutRight(CimdrawScene* scene, const QPointF& delta) const;
    bool shortcutUp(CimdrawScene* scene, const QPointF& delta) const;
    bool shortcutDown(CimdrawScene* scene, const QPointF& delta) const;

    bool cut(CimdrawScene* scene) const;
    bool copy(CimdrawScene* scene) const;
    bool paste(CimdrawScene* scene) const;
    bool remove(CimdrawScene* scene) const;
    bool levelUp(CimdrawScene* scene) const;
    bool levelDown(CimdrawScene* scene) const;
    bool toTop(CimdrawScene* scene) const;
    bool toBottom(CimdrawScene* scene) const;
    bool rotate(CimdrawScene* scene, qreal angleDegrees) const;
    bool group(CimdrawScene* scene) const;
    bool ungroup(CimdrawScene* scene) const;
};

#endif
