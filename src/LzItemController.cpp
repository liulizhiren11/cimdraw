#include "LzItemController.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QUndoStack>
#include <QDebug>
#include <QSignalBlocker>
#include "qtpropertybrowser/qttreepropertybrowser_p.h"
#include "qtpropertybrowser/qtvariantproperty_p.h"
#include "Command/LzValueChangeCommand.h"
#include "LzScene.h"
#include "LzView.h"
#include <QProperty>

namespace {

QString lzFriendlyClassName(const QMetaObject* metaObject)
{
    if (!metaObject)
        return QString();

    const QString className = QString::fromLatin1(metaObject->className());
    if (className == QStringLiteral("LzWiringItemBase"))
        return QStringLiteral("图元属性");
    if (className == QStringLiteral("LzItem"))
        return QStringLiteral("基础属性");
    return className;
}

QString lzFriendlyPropertyName(const QString& propertyName)
{
    static const QMap<QString, QString> kNames = {
        { QStringLiteral("referenceDesignator"), QStringLiteral("位号") },
        { QStringLiteral("displayName"), QStringLiteral("显示名称") },
        { QStringLiteral("valueText"), QStringLiteral("参数文本") },
        { QStringLiteral("modelName"), QStringLiteral("模型名称") },
        { QStringLiteral("wiringDataKey"), QStringLiteral("数据键") },
        { QStringLiteral("wiringRunState"), QStringLiteral("运行状态") },
        { QStringLiteral("switchPosition"), QStringLiteral("开关位置") },
        { QStringLiteral("flowSign"), QStringLiteral("流向") },
        { QStringLiteral("position"), QStringLiteral("位置") },
        { QStringLiteral("rotate"), QStringLiteral("旋转角度") },
        { QStringLiteral("itemPenColor"), QStringLiteral("线色") },
        { QStringLiteral("itemPenWidth"), QStringLiteral("线宽") },
        { QStringLiteral("itemBrushColor"), QStringLiteral("填充色") },
        { QStringLiteral("itemFont"), QStringLiteral("字体") },
    };
    return kNames.value(propertyName, propertyName);
}

} // namespace


class LzItemControllerPrivate
{
    Q_DECLARE_PUBLIC(LzItemController)
public:
    LzItemControllerPrivate(LzView* view,LzItemController* obj)
        :object(nullptr),browser(nullptr),q_ptr(obj),view(view)
        ,manager(nullptr),readOnlyManager(nullptr)
    {
    }
    
    /**
     * @brief    更新类属性
     * @param    [入参:]metaObject 元对象 recursive 是否递归
     * @date     2025-04-30
    */
    void updateClassProperties(const QMetaObject *metaObject, bool recursive);
    
    /**
     * @brief    添加类属性
     * @param    [入参:]metaObject 元对象
     * @date     2025-04-30
    */
    void addClassProperties(const QMetaObject *metaObject);
    
    /**
     * @brief    值改变槽函数
     * @param    [入参:]property 属性 value 值
     * @date     2025-04-30
    */
    void slotValueChanged(QtProperty* property, const QVariant &value);

    /**
     * @brief    枚举类型转换为整型
     * @param    [入参:]metaEnum 元枚举 value 枚举值
     * @date     2025-04-30
    */
    int enumToInt(const QMetaEnum &metaEnum, int enumValue) const;
    
    /**
     * @brief    整型转换为枚举类型
     * @param    [入参:]metaEnum 元枚举 value 整型值
     * @date     2025-04-30
    */
    int intToEnum(const QMetaEnum &metaEnum, int intValue) const;
    
    /**
     * @brief    标志类型转换为整型
     * @param    [入参:]metaEnum 元枚举 value 标志值
     * @date     2025-04-30
    */
    int flagToInt(const QMetaEnum &metaEnum, int flagValue) const;
    
    /**
     * @brief    整型转换为标志类型
     * @param    [入参:]metaEnum 元枚举 value 整型值
     * @date     2025-04-30
    */
    int intToFlag(const QMetaEnum &metaEnum, int intValue) const;
    
    /**
     * @brief    判断子值
     * @param    [入参:]value 值 subValue 子值
     * @date     2025-04-30
    */
    bool isSubValue(int value, int subValue) const;
    
    /**
     * @brief    判断是否为2的幂
     * @param    [入参:]value 值
     * @date     2025-04-30
    */
    bool isPowerOf2(int value) const;

    QObject* object;
    QList<QtProperty*> properties;
    QtAbstractPropertyBrowser* browser;
    QtVariantPropertyManager* manager;
    QtVariantPropertyManager* readOnlyManager;

    QMap<const QMetaObject*, QtProperty*> classToProperty;
    QMap<QtProperty*, const QMetaObject*> propertyToClass;
    QMap<QtProperty*, int> propertyToIndex;
    QMap<const QMetaObject*, QMap<int, QtVariantProperty*>> classToIndexToProperty;
    
    LzView* view;
    LzItemController* q_ptr;
    bool syncingProperties = false;
};


int LzItemControllerPrivate::enumToInt(const QMetaEnum &metaEnum, int enumValue) const
{
    QMap<int, int> valueMap; // dont show multiple enum values which have the same values
    int pos = 0;
    for (int i = 0; i < metaEnum.keyCount(); i++)
    {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value))
        {
            if (value == enumValue)
            {
                return pos;
            }
            valueMap[value] = pos++;
        }
    }
    return -1;
}

int LzItemControllerPrivate::intToEnum(const QMetaEnum &metaEnum, int intValue) const
{
    QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
    QList<int> values;
    for (int i = 0; i < metaEnum.keyCount(); i++)
    {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value))
        {
            valueMap[value] = true;
            values.append(value);
        }
    }
    if (intValue >= values.count())
    {
        return -1;
    }
    return values.at(intValue);
}

bool LzItemControllerPrivate::isSubValue(int value, int subValue) const
{
    if (value == subValue)
    {
        return true;
    }
    int i = 0;
    while (subValue)
    {
        if (!(value & (1 << i)))
        {
            if (subValue & 1)
            {
                return false;
            }
        }
        i++;
        subValue = subValue >> 1;
    }
    return true;
}

bool LzItemControllerPrivate::isPowerOf2(int value) const
{
    while (value)
    {
        if (value & 1)
        {
            return value == 1;
        }
        value = value >> 1;
    }
    return false;
}

int LzItemControllerPrivate::flagToInt(const QMetaEnum &metaEnum, int flagValue) const
{
    if (!flagValue)
    {
        return 0;
    }
    int intValue = 0;
    QMap<int, int> valueMap; // dont show multiple enum values which have the same values
    int pos = 0;
    for (int i = 0; i < metaEnum.keyCount(); i++)
    {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value) && isPowerOf2(value))
        {
            if (isSubValue(flagValue, value))
            {
                intValue |= (1 << pos);
            }
            valueMap[value] = pos++;
        }
    }
    return intValue;
}

int LzItemControllerPrivate::intToFlag(const QMetaEnum &metaEnum, int intValue) const
{
    QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
    QList<int> values;
    for (int i = 0; i < metaEnum.keyCount(); i++)
    {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value) && isPowerOf2(value))
        {
            valueMap[value] = true;
            values.append(value);
        }
    }
    int flagValue = 0;
    int temp = intValue;
    int i = 0;
    while (temp)
    {
        if (i >= values.count())
        {
            return -1;
        }
        if (temp & 1)
        {
            flagValue |= values.at(i);
        }
        i++;
        temp = temp >> 1;
    }
    return flagValue;
}

void LzItemControllerPrivate::slotValueChanged(QtProperty* property, const QVariant &value)
{
    if (syncingProperties)
        return;

    if (!propertyToIndex.contains(property))
    {
        return;
    }

    int idx = propertyToIndex.value(property);

    const QMetaObject* metaObject = object->metaObject();
    QMetaProperty metaProperty = metaObject->property(idx);

    QVariant oldValue = metaProperty.read(object);
    if (metaProperty.isEnumType())
    {
        if (metaProperty.isFlagType())
        {
            metaProperty.write(object, intToFlag(metaProperty.enumerator(), value.toInt()));
        }
        else
        {
            metaProperty.write(object, intToEnum(metaProperty.enumerator(), value.toInt()));
        }
    }
    else
    {
        metaProperty.write(object, value);
    }
    updateClassProperties(metaObject, true);
    if(view)
    {
        LzScene* scene = view->getScene();
        QUndoCommand* command = new LzValueChangeCommand(scene,object,idx,oldValue,value);
        view->getStack()->push(command);
        emit view->editChanged();
    }
}

void LzItemControllerPrivate::updateClassProperties(const QMetaObject *metaObject, bool recursive)
{
    if (!metaObject)
    {
        return;
    }

    const QSignalBlocker blocker(manager);
    syncingProperties = true;

    if (recursive)
    {
        updateClassProperties(metaObject->superClass(), recursive);
    }

    for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++)
    {
        QMetaProperty metaProperty = metaObject->property(idx);
        if (metaProperty.isReadable())
        {
            if (classToIndexToProperty.contains(metaObject) && classToIndexToProperty[metaObject].contains(idx))
            {
                QtVariantProperty* subProperty = classToIndexToProperty[metaObject][idx];
                if (metaProperty.isEnumType())
                {
                    if (metaProperty.isFlagType())
                    {
                        subProperty->setValue(flagToInt(metaProperty.enumerator(), metaProperty.read(object).toInt()));
                    }
                    else
                    {
                        subProperty->setValue(enumToInt(metaProperty.enumerator(), metaProperty.read(object).toInt()));
                    }
                }
                else
                {
                    subProperty->setValue(metaProperty.read(object));
                }
            }
        }
    }

    syncingProperties = false;
}

void LzItemControllerPrivate::addClassProperties(const QMetaObject *metaObject)
{
    if(!metaObject)
    {
        return;
    }
    const QSignalBlocker blocker(manager);
    syncingProperties = true;
    QtProperty* classProperty = classToProperty.value(metaObject);
    if (!classProperty)
    {
        QString className = lzFriendlyClassName(metaObject);
        classProperty = manager->addProperty(QtVariantPropertyManager::groupTypeId(), className);
        classToProperty[metaObject] = classProperty;
        propertyToClass[classProperty] = metaObject;

        for (int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i)
        {
            QMetaProperty metaProperty = metaObject->property(i);
            int type = metaProperty.userType();
            QtVariantProperty *subProperty = nullptr;
            const QString propertyLabel = lzFriendlyPropertyName(QString::fromLatin1(metaProperty.name()));

            // 处理不可读属性
            if (!metaProperty.isReadable())
            {
                subProperty = readOnlyManager->addProperty(QVariant::String, propertyLabel);
                if(subProperty)
                {
                    subProperty->setValue(QString::fromLatin1("< Non Readable >"));
                    if(auto variantProp = dynamic_cast<QtVariantProperty*>(subProperty))
                    {
                        variantProp->setEnabled(false);
                    }
                }
            }
            // 处理枚举类型属性
            else if (metaProperty.isEnumType())
            {
                if (metaProperty.isFlagType())
                {
                    subProperty = manager->addProperty(QtVariantPropertyManager::flagTypeId(), propertyLabel);
                    if(subProperty)
                    {
                        QMetaEnum metaEnum = metaProperty.enumerator();
                        QMap<int, bool> valueMap;
                        QStringList flagNames;
                        for (int i = 0; i < metaEnum.keyCount(); i++)
                        {
                            int value = metaEnum.value(i);
                            if (!valueMap.contains(value) && isPowerOf2(value))
                            {
                                valueMap[value] = true;
                                flagNames.append(QString::fromLatin1(metaEnum.key(i)));
                            }
                        }
                        subProperty->setAttribute(QString::fromLatin1("flagNames"), flagNames);
                        subProperty->setValue(flagToInt(metaEnum, metaProperty.read(object).toInt()));
                    }
                } 
                else
                {
                    subProperty = manager->addProperty(QtVariantPropertyManager::enumTypeId(), propertyLabel);
                    if(subProperty)
                    {
                        QMetaEnum metaEnum = metaProperty.enumerator();
                        QMap<int, bool> valueMap;
                        QStringList enumNames;
                        for (int i = 0; i < metaEnum.keyCount(); i++)
                        {
                            int value = metaEnum.value(i);
                            if (!valueMap.contains(value))
                            {
                                valueMap[value] = true;
                                enumNames.append(QString::fromLatin1(metaEnum.key(i)));
                            }
                        }
                        subProperty->setAttribute(QString::fromLatin1("enumNames"), enumNames);
                        subProperty->setValue(enumToInt(metaEnum, metaProperty.read(object).toInt()));
                    }
                }
            }
            // 处理支持的属性类型
            else if (manager->isPropertyTypeSupported(type))
            {
                if (type == QMetaType::QDate || type == QMetaType::QDateTime || type == QMetaType::QTime)
                {
                    // 对于日期时间类型，使用字符串类型来显示
                    subProperty = manager->addProperty(QVariant::String, propertyLabel);
                    if(subProperty)
                    {
                        QVariant value = metaProperty.read(object);
                        subProperty->setValue(value.toString());
                    }
                }
                else
                {
                    // 其他类型保持原样
                    subProperty = manager->addProperty(type, propertyLabel);
                    if(subProperty)
                    {
                        subProperty->setValue(metaProperty.read(object));
                    }
                }
            }
            // 处理未知类型
            else
            {
                subProperty = readOnlyManager->addProperty(QVariant::String, propertyLabel);
                if(subProperty)
                {
                    subProperty->setValue(QString::fromLatin1("< Unknown Type >"));
                    if(auto variantProp = dynamic_cast<QtVariantProperty*>(subProperty))
                    {
                        variantProp->setEnabled(false);
                    }
                }
            }

            if(subProperty)
            {
                classProperty->addSubProperty(subProperty);
                propertyToIndex[subProperty] = i;
                if(auto variantProp = dynamic_cast<QtVariantProperty*>(subProperty))
                {
                    classToIndexToProperty[metaObject][i] = variantProp;
                }
            }
        }
    }
    else
    {
        updateClassProperties(metaObject,true);
    }

    properties.append(classProperty);
    browser->addProperty(classProperty);
    syncingProperties = false;
}


LzItemController::LzItemController(QWidget *parent , LzView* view)
    : QWidget(parent),d_ptr(new LzItemControllerPrivate(view ,this))
{
    Q_D(LzItemController);
    QtTreePropertyBrowser* browser = new QtTreePropertyBrowser(this);
    //不显示图标或装饰
    browser->setRootIsDecorated(false);
    d->browser = browser;

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(d->browser);

    d->readOnlyManager = new QtVariantPropertyManager(this);

    d->manager = new QtVariantPropertyManager(this);

    QtVariantEditorFactory *factory = new QtVariantEditorFactory(this);
    d->browser->setFactoryForManager(d->manager, factory);

    connect(d->manager, &QtVariantPropertyManager::valueChanged,
            this, &LzItemController::slotValueChanged);
}

LzItemController::~LzItemController()
{

}

void LzItemController::setObject(QObject *object)
{
    Q_D(LzItemController);

    if (d->object)
    {
        QListIterator<QtProperty*> it(d->properties);
        while (it.hasNext())
        {
            d->browser->removeProperty(it.next());
        }
        d->properties.clear();
        d->classToProperty.clear();
        d->propertyToClass.clear();
        d->readOnlyManager->clear();
        d->manager->clear();
        d->propertyToIndex.clear();
        d->classToIndexToProperty.clear();
    }

    d->object = object;
    if (d->view && d->view->getScene())
    {
        connect(d->view->getScene(), &LzScene::itemPropertyChanged,
                this, &LzItemController::updateProperties,Qt::UniqueConnection);
    }
    if (!d->object)
    {
        return;
    }

    d->addClassProperties(object->metaObject());
}


QObject* LzItemController::getObject() const
{
    Q_D(const LzItemController);
    return d->object;
}

void LzItemController::setView(LzView* view)
{
    Q_D(LzItemController);
    d->view = view;
}

LzView* LzItemController::getView() const
{
    Q_D(const LzItemController);
    return d->view;
}

void LzItemController::slotValueChanged(QtProperty* property, const QVariant &value)
{
    Q_D(LzItemController);
    d->slotValueChanged(property, value);
}

void LzItemController::updateProperties()
{
    Q_D(LzItemController);
    if (d->object)
    {
        d->updateClassProperties(d->object->metaObject(), true);
    }
}
