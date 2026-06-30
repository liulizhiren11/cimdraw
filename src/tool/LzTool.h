#ifndef LZTOOL_H
#define LZTOOL_H

#include <QObject>
#include <QString>
#include <QPointF>
#include <QScopedPointer>
#include <QGraphicsSceneMouseEvent>
#include <QIcon>
#include <QVariant>
#include <QPainter>
#include <mutex>

#include "LzItemConfig.h"
#include "Item/TmpBase.h"

class LzScene;
class LzConnectPoint;
class LzListWidgetItem;

class LzTool : public QObject
{
    Q_OBJECT
public:
    explicit LzTool(QObject* parent);
    virtual ~LzTool();

    LzTool(const LzTool&) = delete;
    LzTool& operator=(const LzTool&) = delete;
public:
    /**
     * @brief    创建对象
     * @param    [入参:]scene 场景 pos 位置 addToScene 是否添加到场景
     * @date     2025-04-30
    */
    virtual QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) = 0;

    /**
     * @brief    获取图标
     * @param    [入参:]size 大小 para 参数
     * @date     2025-04-30
    */
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const;

    /**
     * @brief    获取名称
     * @param    [入参:]
     * @date     2025-04-30
    */
    virtual QString getName() const;

    /**
     * @brief    获取绘制类型
     * @return  LzDrawTypeId
     * @date     2025-04-30
    */
    virtual LzDrawTypeId getDrawType() const;

    /**
     * @brief    获取创建模式
     * @return  CREATE_MODE 
     * @date     2025-04-30
    */
    CREATE_MODE getCreateMode() const;

    /**
     * @brief    鼠标按下事件
     * @param    [入参:]scene 场景 evt 事件
     * @date     2025-04-30
    */
    virtual void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt);
    
    /**
     * @brief    鼠标释放事件
     * @param    [入参:]scene 场景 evt 事件
     * @date     2025-04-30
    */
    virtual void onMouseRelease(LzScene* scene, QGraphicsSceneMouseEvent* evt);
    
    /**
     * @brief    鼠标移动事件
     * @param    [入参:]scene 场景 evt 事件
     * @date     2025-04-30
    */
    virtual void onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent* evt);
    
    /**
     * @brief    鼠标双击事件
     * @param    [入参:]scene 场景 evt 事件
     * @date     2025-04-30
    */
    virtual void onMouseDoubleClick(LzScene* scene,QGraphicsSceneMouseEvent* evt);
    
    /**
     * @brief    设置场景
     * @param    [入参:]scene
     * @date     2025-04-30
    */  
    void setScene(LzScene* scene);

    /**
     * @brief    获取场景
     * @return   [入参:]LzScene* 
     * @date     2025-04-30
    */
    LzScene* getScene() const;

    /**
     * @brief    查找图元
     * @param    [入参:]scene 场景 position 位置
     * @return   TmpShape* 
     * @date     2025-04-30
    */
    TmpShape* findShapeAtPosition(LzScene* scene, const QPointF& position);
    /** Alt+点击：自下而上选取（便于选中被图元挡住的连接线） */
    TmpShape* findShapeBehindAtPosition(LzScene* scene, const QPointF& position);
    static bool shapeHitByScenePoint(TmpShape* shape, const QPointF& position);
    static bool shapeHitBySceneRect(TmpShape* shape, const QRectF& rect, bool requireContained = false);

    /**
     * @brief    查找连接点
     * @param createBusbarConnectPoint 为 true 时母线沿边点击可新建端口（仅连线工具）
     */
    LzConnectPoint* findPointAtPosition(LzScene* scene, const QPointF& position,
                                        bool createBusbarConnectPoint = false,
                                        LzConnectPoint* excludeConnectPoint = nullptr);

    /** Ctrl 连线：在图元上取距光标最近的端口（不必点在端口热区上） */
    LzConnectPoint* nearestConnectPointOnShape(TmpShape* shape, const QPointF& scenePos,
                                               qreal maxDist) const;

    /** 场景坐标处是否命中 8 向缩放手柄（dir 1..7） */
    static bool hitResizeHandleAt(LzScene* scene, const QPointF& position);

protected:
    QString name;
    LzDrawTypeId drawType;
    SELECT_MODE currentSelectMode;
    int currentDragHandle;
    LzScene* scene;
    CREATE_MODE createMode;
};

class LzToolManager : public QObject
{
    Q_OBJECT
public:
    static LzToolManager* getInstance();
    ~LzToolManager();
public:
    /**
     * @brief    切换工具
     * @param    [入参:]typeName 名称
     * @date     2025-04-30
    */
    LzTool* changeTool(const QString& typeName);

    /**
     * @brief    切换工具
     * @param    [入参:]type 类型
     * @date     2025-04-30
    */
    LzTool* changeTool(LzDrawTypeId type);

    /**
     * @brief    获取当前工具
     * @date     2025-04-30
    */
    LzTool* getCurrentTool() const;

    /**
     * @brief    设置当前工具
     * @param    [入参:]tool 工具
     * @date     2025-04-30
    */
    void setCurrentTool(LzTool* tool);

private:
    LzToolManager();
    static LzTool* currentTool;
    QMap<QString, LzTool*> registerMap;
};


#endif
