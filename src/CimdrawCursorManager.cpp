#include "CimdrawCursorManager.h"
#include <QPixmap>

CimdrawCursorManager* CimdrawCursorManager::m_instance = nullptr;

CimdrawCursorManager* CimdrawCursorManager::instance()
{
    if (!m_instance) {
        m_instance = new CimdrawCursorManager();
    }
    return m_instance;
}

CimdrawCursorManager::CimdrawCursorManager() : QObject(nullptr)
{
    initialize();
}

void CimdrawCursorManager::initialize()
{
    m_cursors[CimdrawCursorTypeRegistry::CURSOR_DEFAULT] = QCursor(Qt::ArrowCursor);
    
    addCursor(CimdrawCursorTypeRegistry::CURSOR_SELECT, ":/cursors/select.png", 0, 0);
    addCursor(CimdrawCursorTypeRegistry::CURSOR_DRAW, ":/cursors/draw.png", 0, 0);
    addCursor(CimdrawCursorTypeRegistry::CURSOR_ROTATE, ":/cursors/rotate.png", 15, 15);
    addCursor(CimdrawCursorTypeRegistry::CURSOR_RESIZE, ":/cursors/resize.png", 15, 15);
    addCursor(CimdrawCursorTypeRegistry::CURSOR_MOVE, ":/cursors/move.png", 15, 15);
    addCursor(CimdrawCursorTypeRegistry::CURSOR_HAND, ":/cursors/hand.png", 5, 0);
    addCursor(CimdrawCursorTypeRegistry::CURSOR_TEXT, ":/cursors/text.png", 15, 15);
    addCursor(CimdrawCursorTypeRegistry::CURSOR_HOVER, ":/cursors/hover.png", 15, 15);
}

QCursor CimdrawCursorManager::getCursor(int cursorTypeId)
{
    return m_cursors.value(cursorTypeId, QCursor(Qt::ArrowCursor));
}

void CimdrawCursorManager::addCursor(int cursorTypeId, const QString& path, int hotX, int hotY)
{
    QPixmap pixmap(path);
    if (!pixmap.isNull())
    {
        m_cursors[cursorTypeId] = QCursor(pixmap, hotX, hotY);
    }
}

void CimdrawCursorManager::updateCursor(int cursorTypeId, const QString& path, int hotX, int hotY)
{
    addCursor(cursorTypeId, path, hotX, hotY);
}

void CimdrawCursorManager::registerCursor(const QString& name, const QString& resourcePath, int hotX, int hotY)
{
    int cursorTypeId = CimdrawCursorTypeRegistry::instance()->registerCursorType(name);
    addCursor(cursorTypeId, resourcePath, hotX, hotY);
}
