#include "LzCursorManager.h"
#include <QPixmap>

LzCursorManager* LzCursorManager::m_instance = nullptr;

LzCursorManager* LzCursorManager::instance()
{
    if (!m_instance) {
        m_instance = new LzCursorManager();
    }
    return m_instance;
}

LzCursorManager::LzCursorManager() : QObject(nullptr)
{
    initialize();
}

void LzCursorManager::initialize()
{
    m_cursors[LzCursorTypeRegistry::CURSOR_DEFAULT] = QCursor(Qt::ArrowCursor);
    
    addCursor(LzCursorTypeRegistry::CURSOR_SELECT, ":/cursors/select.png", 0, 0);
    addCursor(LzCursorTypeRegistry::CURSOR_DRAW, ":/cursors/draw.png", 0, 0);
    addCursor(LzCursorTypeRegistry::CURSOR_ROTATE, ":/cursors/rotate.png", 15, 15);
    addCursor(LzCursorTypeRegistry::CURSOR_RESIZE, ":/cursors/resize.png", 15, 15);
    addCursor(LzCursorTypeRegistry::CURSOR_MOVE, ":/cursors/move.png", 15, 15);
    addCursor(LzCursorTypeRegistry::CURSOR_HAND, ":/cursors/hand.png", 5, 0);
    addCursor(LzCursorTypeRegistry::CURSOR_TEXT, ":/cursors/text.png", 15, 15);
    addCursor(LzCursorTypeRegistry::CURSOR_HOVER, ":/cursors/hover.png", 15, 15);
}

QCursor LzCursorManager::getCursor(int cursorTypeId)
{
    return m_cursors.value(cursorTypeId, QCursor(Qt::ArrowCursor));
}

void LzCursorManager::addCursor(int cursorTypeId, const QString& path, int hotX, int hotY)
{
    QPixmap pixmap(path);
    if (!pixmap.isNull())
    {
        m_cursors[cursorTypeId] = QCursor(pixmap, hotX, hotY);
    }
}

void LzCursorManager::updateCursor(int cursorTypeId, const QString& path, int hotX, int hotY)
{
    addCursor(cursorTypeId, path, hotX, hotY);
}

void LzCursorManager::registerCursor(const QString& name, const QString& resourcePath, int hotX, int hotY)
{
    int cursorTypeId = LzCursorTypeRegistry::instance()->registerCursorType(name);
    addCursor(cursorTypeId, resourcePath, hotX, hotY);
}
