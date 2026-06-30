#ifndef LZIDATASOURCE_H
#define LZIDATASOURCE_H

#include <QJsonObject>
#include <QObject>
#include <QString>

/**
 * @brief 外部/模拟数据入口的统一抽象，具体协议在 datamodule/sources 下实现。
 */
class LzIDataSource : public QObject
{
    Q_OBJECT
public:
    explicit LzIDataSource(QObject* parent = nullptr);

    virtual QString typeId() const = 0;

    /** 从配置 JSON 对象读取参数（不含外层的 type/enabled，或由子类自行解析） */
    virtual bool configure(const QJsonObject& o) = 0;

    virtual bool start() = 0;
    virtual void stop() = 0;

    virtual bool isRunning() const = 0;
};

#endif
