#include "CimdrawSceneContextMenuController.h"

#include <QIcon>
#include <QPoint>
#include <QMenu>
#include <QPixmap>

#include "CimdrawScene.h"

QMenu* CimdrawSceneContextMenuController::createMenu(CimdrawScene* scene) const
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
        QObject::connect(menu, &QMenu::triggered, scene, &CimdrawScene::slotMenuClicked);
    return menu;
}

QMenu* CimdrawSceneContextMenuController::resetMenu(QMenu* existingMenu, CimdrawScene* scene) const
{
    delete existingMenu;
    return createMenu(scene);
}

QMenu* CimdrawSceneContextMenuController::ensureMenu(QMenu* existingMenu, CimdrawScene* scene) const
{
    return existingMenu ? existingMenu : createMenu(scene);
}

QMenu* CimdrawSceneContextMenuController::prepareMenuForDisplay(QMenu* menu, int fixedActionCount) const
{
    trimTransientActions(menu, fixedActionCount);
    return menu;
}

QAction* CimdrawSceneContextMenuController::execMenu(QMenu* menu, const QPoint& screenPos, int fixedActionCount) const
{
    menu = prepareMenuForDisplay(menu, fixedActionCount);
    return menu ? menu->exec(screenPos) : nullptr;
}

void CimdrawSceneContextMenuController::trimTransientActions(QMenu* menu, int fixedActionCount) const
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
