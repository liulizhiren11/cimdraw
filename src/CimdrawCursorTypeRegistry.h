#ifndef CIMDRAWCURSORTYPEREGISTRY_H
#define CIMDRAWCURSORTYPEREGISTRY_H

#include <QObject>
#include <QMap>
#include <QString>

class CimdrawCursorTypeRegistry
{
public:
    static CimdrawCursorTypeRegistry* instance();

    // 注册新的光标类型
    int registerCursorType(const QString& name);
    
    // 获取光标类型对应的ID
    int getCursorTypeId(const QString& name) const;
    
    // 获取光标类型名称
    QString getCursorTypeName(int id) const;

    // 预定义的光标类型ID常量
    static const int CURSOR_DEFAULT;
    static const int CURSOR_SELECT;
    static const int CURSOR_DRAW;
    static const int CURSOR_ROTATE;
    static const int CURSOR_RESIZE;
    static const int CURSOR_MOVE;
    static const int CURSOR_HAND;
    static const int CURSOR_TEXT;
    static const int CURSOR_HOVER;
    static const int CURSOR_EDIT;

private:
    CimdrawCursorTypeRegistry();
    void initializeDefaultTypes();

    QMap<QString, int> m_nameToId;
    QMap<int, QString> m_idToName;
    int m_nextId;

    static CimdrawCursorTypeRegistry* m_instance;
private:
    Q_DISABLE_COPY(CimdrawCursorTypeRegistry)
};
#endif
