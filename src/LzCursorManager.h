#ifndef LZCURSORMANAGER_H
#define LZCURSORMANAGER_H

#include <QObject>
#include <QCursor>
#include <QMap>
#include "LzCursorTypeRegistry.h"

class LzCursorManager : public QObject
{
    Q_OBJECT

public:
    static LzCursorManager* instance();
    
    // 获取光标
    QCursor getCursor(int cursorTypeId);
    
    // 初始化
    void initialize();
    
    // 添加新光标
    void addCursor(int cursorTypeId, const QString& path, int hotX = 0, int hotY = 0);
    
    // 更新现有光标
    void updateCursor(int cursorTypeId, const QString& path, int hotX = 0, int hotY = 0);
    
    // 注册并添加新光标
    void registerCursor(const QString& name, const QString& resourcePath, int hotX = 0, int hotY = 0);

private:
    LzCursorManager();
    ~LzCursorManager() = default;
    
    QMap<int, QCursor> m_cursors;
    static LzCursorManager* m_instance;
};

#endif // LZ_CURSOR_MANAGER_H