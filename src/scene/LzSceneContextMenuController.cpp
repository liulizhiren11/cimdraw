#include "LzSceneContextMenuController.h"

#include <QIcon>
#include <QPoint>
#include <QMenu>
#include <QPixmap>

#include "LzScene.h"

QMenu* LzSceneContextMenuController::createMenu(LzScene* scene) const
{
    QMenu* menu = new QMenu;

    QAction* cutAction = menu->addAction(QObject::tr("剪切"));
    cutAction->setIcon(QIcon(QPixmap(":/image/edit_cut.png")));

    QAction* copyAction = menu->addAction(QObject::tr("复制"));
    copyAction->setIcon(QIcon(QPixmap(":/image/edit_copy.png")));

    QAction* pasteAction = menu->addAction(QObject::tr("粘贴"));
    pasteAction->setIcon(QIcon(QPixmap(":/image/edit_paste.png")));

    QAction* deleteAction = menu->addAction(QObject::tr("删除"));
    deleteAction->setIcon(QIcon(QPixmap(":/image/edit_delete.png")));

    QAction* groupAction = menu->addAction(QObject::tr("组合"));
    groupAction->setIcon(QIcon(QPixmap(":/image/group.png")));

    QAction* ungroupAction = menu->addAction(QObject::tr("取消组合"));
    ungroupAction->setIcon(QIcon(QPixmap(":/image/ungroup.png")));

    if (scene)
        QObject::connect(menu, &QMenu::triggered, scene, &LzScene::slotMenuClicked);
    return menu;
}

QMenu* LzSceneContextMenuController::resetMenu(QMenu* existingMenu, LzScene* scene) const
{
    delete existingMenu;
    return createMenu(scene);
}

QMenu* LzSceneContextMenuController::ensureMenu(QMenu* existingMenu, LzScene* scene) const
{
    return existingMenu ? existingMenu : createMenu(scene);
}

QMenu* LzSceneContextMenuController::prepareMenuForDisplay(QMenu* menu, int fixedActionCount) const
{
    trimTransientActions(menu, fixedActionCount);
    return menu;
}

QAction* LzSceneContextMenuController::execMenu(QMenu* menu, const QPoint& screenPos, int fixedActionCount) const
{
    menu = prepareMenuForDisplay(menu, fixedActionCount);
    return menu ? menu->exec(screenPos) : nullptr;
}

void LzSceneContextMenuController::trimTransientActions(QMenu* menu, int fixedActionCount) const
{
    if (!menu)
        return;

    QList<QAction*> actions = menu->actions();
    if (actions.count() <= fixedActionCount)
        return;

    for (int i = fixedActionCount; i < actions.count(); ++i)
    {
        QAction* actionToRemove = actions.at(i);
        menu->removeAction(actionToRemove);
        delete actionToRemove;
    }
}
