#ifndef CIMDRAWCURSORMANAGER_H
#define CIMDRAWCURSORMANAGER_H

#include <QObject>
#include <QCursor>
#include <QMap>
#include "CimdrawCursorTypeRegistry.h"

class CimdrawCursorManager : public QObject
{
    Q_OBJECT

public:
    static CimdrawCursorManager* instance();
    
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
    CimdrawCursorManager();
    ~CimdrawCursorManager() = default;
    
    QMap<int, QCursor> m_cursors;
    static CimdrawCursorManager* m_instance;
};

#endif // CIMDRAW_CURSOR_MANAGER_H