#ifndef CIMDRAWSCENECONTEXTMENUCONTROLLER_H
#define CIMDRAWSCENECONTEXTMENUCONTROLLER_H

class QAction;
class QMenu;
class CimdrawScene;
class QPoint;

class CimdrawSceneContextMenuController
{
public:
    QMenu* createMenu(CimdrawScene* scene) const;
    QMenu* resetMenu(QMenu* existingMenu, CimdrawScene* scene) const;
    QMenu* ensureMenu(QMenu* existingMenu, CimdrawScene* scene) const;
    QMenu* prepareMenuForDisplay(QMenu* menu, int fixedActionCount = 6) const;
    QAction* execMenu(QMenu* menu, const QPoint& screenPos, int fixedActionCount = 6) const;
    void trimTransientActions(QMenu* menu, int fixedActionCount = 6) const;
};

#endif
