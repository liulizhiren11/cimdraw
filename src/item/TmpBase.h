#ifndef TMPBASE_H
#define TMPBASE_H

#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QDomElement>
#include <QVector>
#include <QPainter>
#include "LzHandle.h"
#include "LzAttributeManager.h"
#include "LzConnectionParameters.h"
#include "LzConnectPoint.h"
#include "LzItemConfig.h"
#include "algorithm/LzSnowflakeId.h"
#include "topology/ILzTopologyParticipant.h"
#include "topology/TopologyTypes.h"
#include <type_traits>
#include <cstdint>

class LzTool;
class LzConnectLine;
template <typename BaseType>
class TmpBase;
LzConnectPoint* lzEnsureTmpShapeConnectPointAtScene(TmpBase<QGraphicsItem>* shape, const QPointF& scenePos);
void lzRemoveTmpShapeDynamicConnectPointIfUnused(TmpBase<QGraphicsItem>* shape, LzConnectPoint* port);
void lzPruneStaleConnectPointsOnShape(TmpBase<QGraphicsItem>* shape);
bool lzSnapTmpShapeSceneToRectEdge(const TmpBase<QGraphicsItem>* shape, const QPointF& scenePos, qreal maxDist,
                                   QPointF& anchorScene);

template <typename BaseType = QGraphicsItem>
class TmpBase : public BaseType, public ILzTopologyParticipant
{
    friend LzConnectPoint* lzEnsureTmpShapeConnectPointAtScene(TmpBase<QGraphicsItem>* shape,
                                                               const QPointF& scenePos);
    friend void lzRemoveTmpShapeDynamicConnectPointIfUnused(TmpBase<QGraphicsItem>* shape, LzConnectPoint* port);
    friend void lzPruneStaleConnectPointsOnShape(TmpBase<QGraphicsItem>* shape);
    friend bool lzSnapTmpShapeSceneToRectEdge(const TmpBase<QGraphicsItem>* shape, const QPointF& scenePos,
                                              qreal maxDist, QPointF& anchorScene);

public:
    explicit TmpBase(QGraphicsItem * parent = nullptr)
        :BaseType(parent)
    {
        // 在构造函数初始化列表之后，先设置QGraphicsItem的flags
        QGraphicsItem::GraphicsItemFlags flags = QGraphicsItem::GraphicsItemFlag::ItemIsMovable |
                                                QGraphicsItem::GraphicsItemFlag::ItemIsSelectable |
                                                QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges;
        this->setFlags(flags);
        
        // 设置事件接收相关属性
        this->setAcceptTouchEvents(false);
        this->setAcceptedMouseButtons(Qt::AllButtons);
        
        // 初始化控制点
        handleStruct.handles.reserve(HANDLE_POINT::LEFT);
        for (int i = toInt(HANDLE_POINT::LEFT_TOP); i <= toInt(HANDLE_POINT::LEFT); ++i)
        {
            LzHandle* handle = new LzHandle(this, i,false);
            handleStruct.handles.push_back(handle);
        }
        
        // 设置样式相关属性
        setUsePen(LzAttributeManager::usePen_);
        setItemPenColor(LzAttributeManager::penColor_);
        setItemPenWidth(LzAttributeManager::penWidth_);
        setItemPenStyle(LzAttributeManager::penStyle_);

        setUseBrush(LzAttributeManager::useBrush_);
        setItemBrushColor(LzAttributeManager::brushColor_);
        setItemBrushStyle(LzAttributeManager::brushStyle_);

        setUseFont(LzAttributeManager::useFont_);
    }

    explicit TmpBase(const QRectF& pos, QGraphicsItem* parent = nullptr)
        :BaseType(parent)
    {
        // 在构造函数初始化列表之后，先设置QGraphicsItem的flags
        QGraphicsItem::GraphicsItemFlags flags = QGraphicsItem::GraphicsItemFlag::ItemIsMovable |
                                                QGraphicsItem::GraphicsItemFlag::ItemIsSelectable |
                                                QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges;
        this->setFlags(flags);
        
        // 设置事件接收相关属性
        this->setAcceptTouchEvents(false);
        this->setAcceptedMouseButtons(Qt::AllButtons);
        
        // 初始化控制点
        handleStruct.handles.reserve(HANDLE_POINT::LEFT);
        for (int i = toInt(HANDLE_POINT::LEFT_TOP); i <= toInt(HANDLE_POINT::LEFT); ++i)
        {
            LzHandle *handle = new LzHandle(this, i,false);
            handleStruct.handles.push_back(handle);
        }
        
        // 设置样式相关属性
        setUsePen(LzAttributeManager::usePen_);
        setItemPenColor(LzAttributeManager::penColor_);
        setItemPenWidth(LzAttributeManager::penWidth_);
        setItemPenStyle(LzAttributeManager::penStyle_);

        setUseBrush(LzAttributeManager::useBrush_);
        setItemBrushColor(LzAttributeManager::brushColor_);
        setItemBrushStyle(LzAttributeManager::brushStyle_);

        setUseFont(LzAttributeManager::useFont_);
    }


    /**
         * @brief    初始化画笔样式
         * @param    [入参:]painter 
         * @date     2025-04-30
    */
    virtual void prepare(QPainter* painter)
    {
        QPen c_pen;
        QBrush c_brush;
        QFont c_font;
        if (!usePen)
        {
            painter->setPen(Qt::NoPen);
        }
        else
        {
            c_pen.setColor(itemPenColor);
            c_pen.setWidth(itemPenWidth);
            c_pen.setStyle((Qt::PenStyle)itemPenStyle);
            painter->setPen(c_pen);
        }

        if (!useBrush)
        {
            painter->setBrush(Qt::NoBrush);
        }
        else
        {
            c_brush.setColor(itemBrushColor);
            c_brush.setStyle((Qt::BrushStyle)itemBrushStyle);
            painter->setBrush(c_brush);
        }
        if (!useFont)
        {
            // 空实现
        }
        else
        {
            painter->setFont(c_font);
        }

        // Qt6 风格的抗锯齿设置
        painter->setRenderHint(QPainter::Antialiasing);
    }

    virtual ~TmpBase()
    {

    }

    /**
     * @brief    获取显示名称
     * @param    [入参:]
     * @date     2025-04-30
    */
    virtual QString displayName () const
    {
        return QString("TmpBase");
    }

    /**
     * @brief    控制点移动
     * @param    [入参:]dir 控制点方向 delta 移动距离
     * @date     2025-04-30
    */
    virtual void control(int dir, const QPointF & delta)
    {
        Q_UNUSED(dir);Q_UNUSED(delta);
    }

    /**
     * @brief    拉伸
     * @param    [入参:]dir 拉伸方向 scale 拉伸比例 delta 移动距离
     * @date     2025-04-30
    */
    virtual void stretch(int , double , double , const QPointF &)
    {

    }

    /**
     * @brief    获取矩形
     * @param    [入参:]
     * @date     2025-04-30
    */
    virtual QRectF  rect() const
    {
        return itemPosition;
    }

    /**
     * @brief    更新坐标
     * @param    [入参:]
     * @date     2025-04-30
    */
    virtual void updateCoordinate()
    {

    }

    /**
     * @brief    移动
     * @param    [入参:]delta 移动距离
     * @date     2025-04-30
    */
    virtual void move(const QPointF & delta)
    {
        this->moveBy(delta.x(), delta.y());
        updateConnect(delta);
    }

    /**
     * @brief    复制
     * @param    [入参:]
     * @date     2025-04-30
    */
    virtual QGraphicsItem* duplicate()
    {
        return nullptr;
    }

    /**
     * @brief    获取画笔是否启用
     * @param    [入参:]
     * @date     2025-04-30
    */
    bool getUsePen() const
    {
        return usePen;
    }

    /**
     * @brief    设置画笔是否启用
     * @param    [入参:]value 是否启用
     * @date     2025-04-30
    */
    void setUsePen(bool value)
    {
        usePen = value;
    }

    /**
     * @brief    获取画笔颜色
     * @param    [入参:]
     * @date     2025-04-30
    */
    QColor getItemPenColor() const
    {
        return itemPenColor;
    }

    /**
     * @brief    设置画笔颜色
     * @param    [入参:]color 颜色
     * @date     2025-04-30
    */
    void setItemPenColor(const QColor& color)
    {
        itemPenColor = color;
    }

    /**
     * @brief    获取画笔宽度
     * @param    [入参:]
     * @date     2025-04-30
    */
    int getItemPenWidth() const
    {
        return itemPenWidth;
    }

    /**
     * @brief    设置画笔宽度
     * @param    [入参:]width 宽度
     * @date     2025-04-30
    */
    void setItemPenWidth(int width)
    {
        itemPenWidth = width;
    }

    /**
     * @brief    获取画笔样式
     * @param    [入参:]
     * @date     2025-04-30
    */  
    int getItemPenStyle() const
    {
        return itemPenStyle;
    }

    /**
     * @brief    设置画笔样式
     * @param    [入参:]style 样式
     * @date     2025-04-30
    */
    void setItemPenStyle(int style)
    {
        itemPenStyle = style;
    }

    /**
     * @brief    获取画刷是否启用
     * @param    [入参:]
     * @date     2025-04-30
    */
    bool getUseBrush() const
    {
        return useBrush;
    }

    /**
     * @brief    设置画刷是否启用
     * @param    [入参:]value 是否启用
     * @date     2025-04-30
    */
    void setUseBrush(bool value)
    {
        useBrush = value;
    }

    /**
     * @brief    获取画刷颜色
     * @param    [入参:]
     * @date     2025-04-30
    */
    QColor getItemBrushColor() const
    {
        return itemBrushColor;
    }

    /**
     * @brief    设置画刷颜色
     * @param    [入参:]color 颜色
     * @date     2025-04-30
    */
    void setItemBrushColor(const QColor& color)
    {
        itemBrushColor = color;
    }

    /**
     * @brief    获取画刷样式
     * @param    [入参:]
     * @date     2025-04-30
    */
    int getItemBrushStyle() const
    {
        return itemBrushStyle;
    }

    /**
     * @brief    设置画刷样式
     * @param    [入参:]style 样式
     * @date     2025-04-30
    */
    void setItemBrushStyle(int style)
    {
        itemBrushStyle = style;
    }

    /**
     * @brief    获取字体是否启用
     * @param    [入参:]
     * @date     2025-04-30
    */
    bool getUseFont() const
    {
        return useFont;
    }

    /**
     * @brief    设置字体是否启用
     * @param    [入参:]value 是否启用
     * @date     2025-04-30
    */
    void setUseFont(bool value)
    {
        useFont = value;
    }

    /**
     * @brief    获取字体
     * @param    [入参:]
     * @date     2025-04-30
    */
    QFont getItemFont() const
    {
        return itemFont;
    }

    /**
     * @brief    设置字体
     * @param    [入参:]font 字体
     * @date     2025-04-30
    */
    void setItemFont(const QFont& font)
    {
        itemFont = font;
    }

    /**
     * @brief    设置旋转角度
     * @param    [入参:]data 旋转角度
     * @date     2025-04-30
    */
    void setItemDegree(qreal data)
    {  
        itemDegree = fmod(data, 360.0);
        if (itemDegree < 0)
        {
            itemDegree += 360.0;
        }
        this->setRotation(itemDegree);
    }

    /**
     * @brief    获取旋转角度
     * @param    [入参:]
     * @date     2025-04-30
    */
    qreal getItemDegree()
    {
        return itemDegree;
    }

    /**
     * @brief    获取宽度
     * @param    [入参:]
     * @date     2025-04-30
    */
    qreal getWidth() const
    {
        return itemWidth;
    }

    /**
     * @brief    设置宽度
     * @param    [入参:]width 宽度
     * @date     2025-04-30
    */
    virtual void setWidth(qreal width)
    {
    }

    /**
     * @brief    获取高度
     * @param    [入参:]
     * @date     2025-04-30
    */  
    qreal getHeight()const
    {
        return itemHeight;
    }  

    /**
     * @brief    设置高度
     * @param    [入参:]height 高度
     * @date     2025-04-30
    */
    virtual void setHeight(qreal height)
    {

    }

    /**
     * @brief    设置大小
     * @param    [入参:]size 大小
     * @date     2025-04-30
    */
    virtual void setSize(QSizeF size)
    {

    }

    /**
     * @brief    获取大小
     * @param    [入参:]
     * @date     2025-04-30
    */
    void getSize()
    {

    }

    /**
     * @brief    更新控制点
     * @param    [入参:]
     * @date     2025-04-30
    */
    virtual void updateHandles()
    {
        const QRectF& geom = itemPosition;
        for (auto item : handleStruct.handles)
        {
            LzHandle* handle = item;
            switch (item->dir())
            {
            case toInt(HANDLE_POINT::LEFT_TOP):
            {
                handle->move(geom.x(), geom.y());
            }
                break;
            case toInt(HANDLE_POINT::TOP):
            {
                handle->move(geom.x() + geom.width() / 2, geom.y());
            }
                break;
            case toInt(HANDLE_POINT::RIGHT_TOP):
            {
                handle->move(geom.x() + geom.width(), geom.y());
            }
                break;
            case toInt(HANDLE_POINT::RIGHT):
            {
                handle->move(geom.x() + geom.width(), geom.y() + geom.height() / 2);
            }
                break;
            case toInt(HANDLE_POINT::RIGHT_BOTTOM):
            {
                handle->move(geom.x() + geom.width(), geom.y() + geom.height());
            }
                break;
            case toInt(HANDLE_POINT::BOTTOM):
            {
                handle->move(geom.x() + geom.width() / 2, geom.y() + geom.height());
            }
                break;
            case toInt(HANDLE_POINT::LEFT_BOTTOM):
            {
                handle->move(geom.x(), geom.y() + geom.height());
            }
                break;
            case toInt(HANDLE_POINT::LEFT):
            {
                handle->move(geom.x(), geom.y() + geom.height() / 2);
            }
                break;
            default:
                break;
            }
        }
    }

    /**
     * @brief    更新连接点
     * @param    [入参:]
     * @date     2025-04-30
    */
    virtual void updatePoints()
    {
        for (auto item : pointStruct.points)
        {
            LzConnectPoint* point = item;
            QPointF offset = point->getRelativePos();
            QPointF geom = itemPosition.center();
            point->move(geom.x() + offset.x() * itemWidth,geom.y() + offset.y() * itemHeight);
        }
    }

    /**
     * @brief    设置状态
     * @param    [入参:]st 状态
     * @date     2025-04-30
    */
    virtual void setState(HANDLE_STATE st)
    {
        if (!this->scene())
            return;
        const HandleType::Handles::iterator hend = handleStruct.handles.end();
        for (HandleType::Handles::iterator it = handleStruct.handles.begin(); it != hend; ++it)
        {
            (*it)->setState(st);
        }
    }

    /**
     * @brief    获取控制点位置
     * @param    [入参:]handle 控制点
     * @date     2025-04-30
    */
    virtual QPointF handlePos(int handle) const
    {
        const HandleType::Handles::const_reverse_iterator hend =  handleStruct.handles.rend();
        for (HandleType::Handles::const_reverse_iterator it = handleStruct.handles.rbegin(); it != hend; ++it)
        {
            if ((*it)->dir() == static_cast<int>(handle))
            {
                return (*it)->pos();
            }
        }
        return QPointF();
    }

    /**
     * @brief    获取控制点数量
     * @param    [入参:]
     * @date     2025-04-30
    */
    virtual int handleCount() const 
    { 
        return handleStruct.handles.size();
    }

    /**
     * @brief    判断控制点
     * @param    [入参:]point 点（场景坐标）
     * @date     2025-04-30
    */
    virtual int collidesWithHandle(const QPointF& point) const
	{
        const QVector<LzHandle*>::const_reverse_iterator hend = handleStruct.handles.rend();
        for (QVector<LzHandle*>::const_reverse_iterator it = handleStruct.handles.rbegin(); it != hend; ++it)
		{
			QPointF pt = (*it)->mapFromScene(point);
			if ((*it)->contains(pt)) 
            {
				return (*it)->dir();
			}
		}
		return HANDLE_POINT::HANDLE_NONE;
	}

    /**
     * @brief    交换控制点
     * @param    [入参:]handle 控制点 scale 缩放
     * @date     2025-04-30
    */
    int swapHandle(int handle, const QPointF& scale) const
    {
        int dir = HANDLE_POINT::HANDLE_NONE;
        if ( scale.x() < 0 || scale.y() < 0 )
        {
            switch (handle)
            {
            case HANDLE_POINT::RIGHT_TOP:
                if ( scale.x() < 0 && scale.y() < 0 )
                    dir = HANDLE_POINT::LEFT_BOTTOM;
                else if ( scale.x() > 0 && scale.y() < 0 )
                    dir = HANDLE_POINT::RIGHT_BOTTOM;
                else
                    dir = HANDLE_POINT::LEFT_TOP;
                break;
            case HANDLE_POINT::RIGHT_BOTTOM:
                if ( scale.x() < 0 && scale.y() < 0 )
                    dir = HANDLE_POINT::LEFT_TOP;
                else if ( scale.x() > 0 && scale.y() < 0 )
                    dir = HANDLE_POINT::RIGHT_TOP;
                else
                    dir = HANDLE_POINT::LEFT_BOTTOM;
                break;
            case HANDLE_POINT::LEFT_BOTTOM:
                if ( scale.x() < 0 && scale.y() < 0 )
                    dir = HANDLE_POINT::RIGHT_TOP;
                else if ( scale.x() > 0 && scale.y() < 0 )
                    dir = HANDLE_POINT::LEFT_TOP;
                else
                    dir = HANDLE_POINT::RIGHT_BOTTOM;
                break;
            case HANDLE_POINT::LEFT_TOP:
                if ( scale.x() < 0 && scale.y() < 0 )
                    dir = HANDLE_POINT::RIGHT_BOTTOM;
                else if ( scale.x() > 0 && scale.y() < 0 )
                    dir = HANDLE_POINT::LEFT_BOTTOM;
                else
                    dir = HANDLE_POINT::RIGHT_TOP;
                break;
            case HANDLE_POINT::RIGHT:
                if (scale.x() < 0 )
                    dir = HANDLE_POINT::LEFT;
                break;
            case HANDLE_POINT::LEFT:
                if (scale.x() < 0 )
                    dir = HANDLE_POINT::RIGHT;
                break;
            case HANDLE_POINT::TOP:
                if (scale.y()<0)
                    dir = HANDLE_POINT::BOTTOM;
                break;
            case HANDLE_POINT::BOTTOM:
                if (scale.y()<0)
                    dir = HANDLE_POINT::TOP;
                break;
            }
        }
        return dir;
    }

    /**
     * @brief    获取对应控制点所在的pos
     * @param    [入参:]handle 控制点
     * @date     2025-04-30
    */
    virtual QPointF opposite(int handle)
     {
		QPointF pt;
		switch (static_cast<HANDLE_POINT>(handle)) 
		{
		case HANDLE_POINT::RIGHT:
            pt = handleStruct.handles[toInt(HANDLE_POINT::LEFT) - 1]->pos();
			break;
		case HANDLE_POINT::RIGHT_TOP:
            pt = handleStruct.handles[toInt(HANDLE_POINT::LEFT_BOTTOM) - 1]->pos();
			break;
		case HANDLE_POINT::RIGHT_BOTTOM:
            pt = handleStruct.handles[toInt(HANDLE_POINT::LEFT_TOP) - 1]->pos();
			break;
		case HANDLE_POINT::LEFT_BOTTOM:
            pt = handleStruct.handles[toInt(HANDLE_POINT::RIGHT_TOP) - 1]->pos();
			break;
		case HANDLE_POINT::BOTTOM:
            pt = handleStruct.handles[toInt(HANDLE_POINT::TOP) - 1]->pos();
			break;
		case HANDLE_POINT::LEFT_TOP:
            pt = handleStruct.handles[toInt(HANDLE_POINT::RIGHT_BOTTOM) - 1]->pos();
			break;
		case HANDLE_POINT::LEFT:
            pt = handleStruct.handles[toInt(HANDLE_POINT::RIGHT) - 1]->pos();
			break;
		case HANDLE_POINT::TOP:
            pt = handleStruct.handles[toInt(HANDLE_POINT::BOTTOM) - 1]->pos();
			break;
		}
		return pt;
	}

    /**
     * @brief    设置绘制工具
     * @param    [入参:]tool 工具
     * @date     2025-04-30
    */
    void setDrawTool(LzTool* tool)
    {
        tool = tool;
    }

    /**
     * @brief    获取场景变换原点
     * @param    [入参:]
     * @date     2025-04-30
    */
    QPointF getSceneTransformOriginPoint()
    {
        return this->mapToScene(this->transformOriginPoint());
    }

    /** @brief PDW/XML 持久化的图元唯一标识（雪花式字符串） */
    QString lzObjectId() const { return m_lzObjectId; }
    void setLzObjectId(const QString& id) { m_lzObjectId = id; }
    QString ensureLzObjectId()
    {
        if (m_lzObjectId.isEmpty())
            m_lzObjectId = LzSnowflakeId::instance().nextIdString();
        return m_lzObjectId;
    }
    void writeXmlObjectId(QDomElement* g)
    {
        if (!g)
            return;
        ensureLzObjectId();
        g->setAttribute(QStringLiteral("lzObjectId"), m_lzObjectId);
    }
    void readXmlObjectId(const QDomElement& g)
    {
        if (g.hasAttribute(QStringLiteral("lzObjectId")))
            m_lzObjectId = g.attribute(QStringLiteral("lzObjectId"));
    }

    /** 拓扑节点稳定 ID（一阶段与 lzObjectId 共用存储；XML 另写 topologyNodeId 便于对齐方案文档） */
    QString topologyNodeStableId() const { return m_lzObjectId; }
    void setTopologyNodeStableId(const QString& id) { m_lzObjectId = id; }
    QString ensureTopologyNodeStableId() { return ensureLzObjectId(); }

    /** 是否为拓扑图节点图元（用于连线端点语义与索引） */
    virtual bool isTopologyGraphNode() const { return false; }
    bool participatesInTopology() const override { return false; }
    LzTopologyDomain topologyDomain() const override { return LzTopologyDomain::None; }

    /**
     * @brief    保存xml
     * @param    [入参:]g 元素
     * @date     2025-04-30
    */
    virtual bool saveXml(QDomElement *g)
    {
        return true;
    }

    /**
     * @brief    加载xml
     * @param    [入参:]g 元素
     * @date     2025-04-30
    */
    virtual bool loadXml(QDomElement *g)
    {
        return true;
    }

    /**
     * @brief    获取类名
     * @param    [入参:]
     * @date     2025-04-30
    */
    virtual QString className()
    {
        return "";
    }

    /**
     * @brief    获取形状名称
     * @param    [入参:]
     * @date     2025-04-30
    */
    virtual QString shapeName() const
    {
        return QString();
    }

    /**
     * @brief    获取左边
     * @param    [入参:]
     * @date     2025-04-30
    */      
    virtual qreal itemLeft()
	{
        return 0;
	}

    /**
     * @brief    获取上边
     * @param    [入参:]
     * @date     2025-04-30
    */
	virtual qreal itemTop()
	{
		return 0;
	}

    /**
     * @brief    获取下边
     * @param    [入参:]
     * @date     2025-04-30
    */
	virtual qreal itemBottom()
	{
		return 0;
	}

    /**
     * @brief    获取右边
     * @param    [入参:]
     * @date     2025-04-30
    */
	virtual qreal itemRight()
	{
		return 0;
	}

    /**
     * @brief    获取中心x
     * @param    [入参:]
     * @date     2025-04-30
    */
    virtual qreal centerX()
    {
        return 0;
    }

    /**
     * @brief    获取中心y
     * @param    [入参:]
     * @date     2025-04-30
    */
    virtual qreal centerY()
    {
        return 0;
    }

    /**
     * @brief    设置选中子级
     * @param    [入参:]value 是否选中
     * @date     2025-04-30
    */
    void setSelectChiled(bool value)
    {
        selectChild = value;
    }

    /**
     * @brief    获取选中子级
     * @param    [入参:]
     * @date     2025-04-30
    */
    bool getSelectChiled()
    {
        return selectChild;
    }

    /**
     * @brief    添加连接
     * @param    [入参:]item 图形项
     * @date     2025-04-30
    */
    void addConnect(QGraphicsItem* item)
    {
        if (!item)
            return;
        if(!connectStruct.connects.contains(item))
            connectStruct.connects.append(item);
    }

    /**
     * @brief    移除连接
     * @param    [入参:]item 图形项
     * @date     2025-04-30
    */
    void removeConnect(QGraphicsItem* item)
    {
        if (!item)
            return;
        connectStruct.connects.removeOne(item);
    }

    QVector<QGraphicsItem*> connectedItems() const
    {
        return connectStruct.connects;
    }

    /**
     * @brief    更新连接线
     * @param    [入参:]delta 移动距离
     * @date     2025-04-30
    */
    virtual void updateConnect(const QPointF& delta)
    {

    }

    /**
     * @brief    更新连接线位置
     * @param    [入参:]item 图形项 delta 移动距离
     * @date     2025-04-30
    */
    virtual void updatePosition(QGraphicsItem* item, const QPointF& delta)
    {

    }

    /**
     * @brief    获取连接点位置
     * @param    [入参:]point 点
     * @date     2025-04-30
    */
    virtual QPointF connPointPos(int point) const
    {
        const auto hend =  pointStruct.points.rend();
        for (auto it = pointStruct.points.rbegin(); it != hend; ++it)
        {
            if ((*it)->dir() == point)
            {
                return (*it)->pos();
            }
        }
        return QPointF();
    }

    const QVector<LzConnectPoint*>& wiringConnectPorts() const
    {
        return pointStruct.points;
    }

protected:
    bool     usePen;
    QColor   itemPenColor;
    int      itemPenWidth;
    int      itemPenStyle;

    bool     useBrush;
    QColor   itemBrushColor;
    int      itemBrushStyle;

    bool     useFont;
    QFont    itemFont;

    //旋转角度
    qreal  itemDegree = 0.0;

    //本身
    QRectF itemPosition;
    //
    QRectF itemInitialRect;
    //宽度
    qreal  itemWidth;
    //高度
    qreal  itemHeight;
    //控制点
    HandleType::HandleParameters handleStruct;
    //连接线
    ConnectType::LzConnectionParameters connectStruct;
    //连接点
    ConnectPoint::ConnectPoint pointStruct;
    //是否允许选中内部子级
    bool selectChild = false;

    LzTool* tool = nullptr;

    /** @see lzObjectId() */
    QString m_lzObjectId;
};

static QPainterPath shapeFromPath(const QPainterPath &path, const QPen &pen)
{
    const qreal penWidthSero = qreal(0.0000001);
    if (path == QPainterPath() || pen == Qt::NoPen)
    {
        return path;
    }
    QPainterPathStroker ps;
    ps.setCapStyle(pen.capStyle());
    if (pen.width() <= 0.0)
    {
        ps.setWidth(penWidthSero);
    }
    else
    {
        ps.setWidth(pen.widthF());
    }
    ps.setJoinStyle(pen.joinStyle());
    ps.setMiterLimit(pen.miterLimit());
    QPainterPath p = ps.createStroke(path);
    p.addPath(path);
    return p;
}

using TmpShape = TmpBase<QGraphicsItem>;
#endif
