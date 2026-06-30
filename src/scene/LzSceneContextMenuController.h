#ifndef LZSCENECONTEXTMENUCONTROLLER_H
#define LZSCENECONTEXTMENUCONTROLLER_H

class QAction;
class QMenu;
class LzScene;
class QPoint;

class LzSceneContextMenuController
{
public:
    QMenu* createMenu(LzScene* scene) const;
    QMenu* resetMenu(QMenu* existingMenu, LzScene* scene) const;
    QMenu* ensureMenu(QMenu* existingMenu, LzScene* scene) const;
    QMenu* prepareMenuForDisplay(QMenu* menu, int fixedActionCount = 6) const;
    QAction* execMenu(QMenu* menu, const QPoint& screenPos, int fixedActionCount = 6) const;
    void trimTransientActions(QMenu* menu, int fixedActionCount = 6) const;
};

#endif
