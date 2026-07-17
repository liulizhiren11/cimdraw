#include "CimdrawSnowflakeId.h"

#include <QDateTime>
#include <QNetworkInterface>
#include <QRandomGenerator>
#include <QString>
#include <QThread>

namespace {

QString physicalMacNoColons()
{
    static QString cached;
    if (!cached.isEmpty())
        return cached;

    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface& ni : interfaces) {
        if (!ni.isValid())
            continue;
        if (ni.flags().testFlag(QNetworkInterface::IsLoopBack))
            continue;
        if (!ni.flags().testFlag(QNetworkInterface::IsUp))
            continue;

        const QString hw = ni.hardwareAddress();
        if (hw.isEmpty())
            continue;
        if (hw.compare(QString("00:00:00:00:00:00"), Qt::CaseInsensitive) == 0)
            continue;

        QString s = hw;
        s.remove(QLatin1Char(':'));
        cached = s.toLower();
        return cached;
    }

    const quint64 v = QRandomGenerator::global()->generate64() & 0xFFFFFFFFFFFFULL;
    cached = QString("rnd%1").arg(v, 12, 16, QLatin1Char('0'));
    return cached;
}

} // namespace

CimdrawSnowflakeId& CimdrawSnowflakeId::instance()
{
    static CimdrawSnowflakeId s_inst;
    return s_inst;
}

void CimdrawSnowflakeId::configure(quint16 workerId)
{
    QMutexLocker locker(&m_mutex);
    m_workerId = workerId;
}

QString CimdrawSnowflakeId::nextIdString()
{
    QMutexLocker locker(&m_mutex);

    qint64 ts = QDateTime::currentMSecsSinceEpoch();
    if (ts == m_lastMs) {
        ++m_sameMsSeq;
        if (m_sameMsSeq > 0xFFFu) {
            while (QDateTime::currentMSecsSinceEpoch() <= m_lastMs)
                QThread::usleep(50);
            ts = QDateTime::currentMSecsSinceEpoch();
            m_lastMs = ts;
            m_sameMsSeq = 0;
        }
    } else {
        m_lastMs = ts;
        m_sameMsSeq = 0;
    }

    const QString mac = physicalMacNoColons();
    quint64 salt = QRandomGenerator::global()->generate64();
    salt ^= (static_cast<quint64>(m_workerId) << 40);

    const QString seqHex = QString::number(m_sameMsSeq & 0xFFFu, 16).rightJustified(3, QLatin1Char('0'));
    const QString randHex = QString::number(salt & 0xFFFFFFFFFFFFULL, 16).rightJustified(12, QLatin1Char('0'));

    return QString::number(ts) + QLatin1Char('_') + mac + QLatin1Char('_') + seqHex + QLatin1Char('_') + randHex;
}
