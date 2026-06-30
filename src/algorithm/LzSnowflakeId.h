#ifndef LZSNOWFLAKEID_H
#define LZSNOWFLAKEID_H

#include <QMutex>
#include <QString>
#include <QtGlobal>

/**
 * @brief 拓扑/业务用全局唯一字符串 ID（雪花风格：时间有序 + 机器区分 + 随机）
 *
 * 组成：毫秒时间戳 + 本机物理网卡 MAC（无冒号小写）+ 同毫秒内序号（12bit）+ 48bit 随机十六进制；
 * 无可用网卡时用进程内稳定的 rnd + 12 位十六进制占位段。线程安全。
 */
class LzSnowflakeId
{
public:
    static LzSnowflakeId& instance();

    /// 可选：多实例/多 worker 时区分随机盐（默认 0）
    void configure(quint16 workerId);
    quint16 workerId() const { return m_workerId; }

    /// 生成新的唯一 ID 字符串，可直接写入 XML/JSON
    QString nextIdString();

private:
    LzSnowflakeId() = default;
    LzSnowflakeId(const LzSnowflakeId&) = delete;
    LzSnowflakeId& operator=(const LzSnowflakeId&) = delete;

    mutable QMutex m_mutex;
    qint64 m_lastMs = 0;
    quint32 m_sameMsSeq = 0;
    quint16 m_workerId = 0;
};

#endif // LZSNOWFLAKEID_H
