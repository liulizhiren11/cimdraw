#include "CimdrawCursorTypeRegistry.h"

CimdrawCursorTypeRegistry* CimdrawCursorTypeRegistry::m_instance = nullptr;

const int CimdrawCursorTypeRegistry::CURSOR_DEFAULT = 0;
const int CimdrawCursorTypeRegistry::CURSOR_SELECT = 1;
const int CimdrawCursorTypeRegistry::CURSOR_DRAW = 2;
const int CimdrawCursorTypeRegistry::CURSOR_ROTATE = 3;
const int CimdrawCursorTypeRegistry::CURSOR_RESIZE = 4;
const int CimdrawCursorTypeRegistry::CURSOR_MOVE = 5;
const int CimdrawCursorTypeRegistry::CURSOR_HAND = 6;
const int CimdrawCursorTypeRegistry::CURSOR_TEXT = 7;
const int CimdrawCursorTypeRegistry::CURSOR_HOVER = 8;
const int CimdrawCursorTypeRegistry::CURSOR_EDIT = 9;

CimdrawCursorTypeRegistry* CimdrawCursorTypeRegistry::instance()
{
    if (!m_instance)
    {
        m_instance = new CimdrawCursorTypeRegistry();
    }
    return m_instance;
}

CimdrawCursorTypeRegistry::CimdrawCursorTypeRegistry() : m_nextId(0)
{
    initializeDefaultTypes();
}

void CimdrawCursorTypeRegistry::initializeDefaultTypes()
{
    m_nameToId["DEFAULT"] = CURSOR_DEFAULT;
    m_nameToId["SELECT"] = CURSOR_SELECT;
    m_nameToId["DRAW"] = CURSOR_DRAW;
    m_nameToId["ROTATE"] = CURSOR_ROTATE;
    m_nameToId["RESIZE"] = CURSOR_RESIZE;
    m_nameToId["MOVE"] = CURSOR_MOVE;
    m_nameToId["HAND"] = CURSOR_HAND;
    m_nameToId["TEXT"] = CURSOR_TEXT;
    m_nameToId["HOVER"] = CURSOR_HOVER;
    m_nameToId["EDIT"] = CURSOR_EDIT;

    for (auto it = m_nameToId.begin(); it != m_nameToId.end(); ++it)
    {
        m_idToName[it.value()] = it.key();
    }

    m_nextId = CURSOR_HOVER + 1;
}

int CimdrawCursorTypeRegistry::registerCursorType(const QString& name)
{
    if (m_nameToId.contains(name))
    {
        return m_nameToId[name];
    }

    int newId = m_nextId++;
    m_nameToId[name] = newId;
    m_idToName[newId] = name;
    return newId;
}

int CimdrawCursorTypeRegistry::getCursorTypeId(const QString& name) const
{
    return m_nameToId.value(name, CURSOR_DEFAULT);
}

QString CimdrawCursorTypeRegistry::getCursorTypeName(int id) const
{
    return m_idToName.value(id, "DEFAULT");
}
