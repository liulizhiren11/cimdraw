#ifndef CIMDRAWITEMCONTROLLER_H
#define CIMDRAWITEMCONTROLLER_H

#include <QWidget>
#include <QScopedPointer>
#include <QVariant>

// 改为使用公共头文件
#include "qtpropertybrowser/qtpropertybrowser_p.h"

class CimdrawView;
class QtProperty;

class CimdrawItemControllerPrivate;

class CimdrawItemController : public QWidget
{
    Q_OBJECT
public:
    CimdrawItemController(QWidget* parent = nullptr , CimdrawView* view = nullptr);
    ~CimdrawItemController();

    /**
     * @brief    设置视图
     * @param    [入参:]view 视图
     * @date     2025-04-30
    */
    void setView(CimdrawView* view); 

    /**
     * @brief    获取视图
     * @return  CimdrawView* 
     * @date     2025-04-30
    */
    CimdrawView* getView() const;

    /**
     * @brief    设置当前属性对象
     * @param    [入参:]object
     * @date     2025-04-30
    */
    void setObject(QObject* object);

    /**
     * @brief    获取当前属性对象
     * @return  QObject* 
     * @date     2025-04-30
    */
    QObject* getObject() const;

public slots:
    /**
     * @brief    更新属性
     * @param    [入参:]
     * @date     2025-04-30
    */
    void updateProperties();

    /**
     * @brief    属性值改变
     * @param    [入参:]property 属性 value 值
     * @date     2025-04-30
    */
    void slotValueChanged(QtProperty* property, const QVariant &value);
private:
    QScopedPointer<CimdrawItemControllerPrivate> d_ptr;
    Q_DECLARE_PRIVATE(CimdrawItemController)
};

#endif


