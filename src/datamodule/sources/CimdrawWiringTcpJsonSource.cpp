#include "CimdrawWiringTcpJsonSource.h"
#include "CimdrawScene.h"
#include "cim/behavior/CimBehaviorRuntimeIngest.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QTcpSocket>

#include <QDebug>

CimdrawWiringTcpJsonSource::CimdrawWiringTcpJsonSource(CimdrawSceneResolver resolver, QObject* parent)
    : CimdrawIDataSource(parent)
    , sceneResolver_(std::move(resolver))
{
    server_ = new QTcpServer(this);
    connect(server_, &QTcpServer::newConnection, this, &CimdrawWiringTcpJsonSource::onNewConnection);
}

CimdrawWiringTcpJsonSource::~CimdrawWiringTcpJsonSource()
{
    stop();
}

bool CimdrawWiringTcpJsonSource::configure(const QJsonObject& o)
{
    const QJsonValue p = o.contains(QStringLiteral("listenPort")) ? o.value(QStringLiteral("listenPort"))
                                                                  : o.value(QStringLiteral("port"));
    const int raw = p.toInt(0);
    if (raw <= 0 || raw > 65535)
    {
        qWarning() << "CimdrawWiringTcpJsonSource: invalid listenPort";
        return false;
    }
    port_ = static_cast<quint16>(raw);

    const QString hostStr = o.value(QStringLiteral("listenAddress")).toString(
        o.value(QStringLiteral("bindAddress")).toString(QStringLiteral("127.0.0.1")));
    const QHostAddress addr(hostStr);
    if (addr.isNull())
    {
        qWarning() << "CimdrawWiringTcpJsonSource: invalid listenAddress" << hostStr;
        return false;
    }
    bindAddress_ = addr;
    return true;
}

bool CimdrawWiringTcpJsonSource::start()
{
    if (!server_ || port_ == 0)
        return false;
    if (server_->isListening())
        return true;
    if (!server_->listen(bindAddress_, port_))
    {
        qWarning() << "CimdrawWiringTcpJsonSource: listen failed" << bindAddress_.toString() << port_
                   << server_->errorString();
        return false;
    }
    qInfo() << "CimdrawWiringTcpJsonSource: listening" << server_->serverAddress().toString() << server_->serverPort();
    return true;
}

void CimdrawWiringTcpJsonSource::stop()
{
    if (server_ && server_->isListening())
        server_->close();
    const auto socks = recvBuffers_.keys();
    for (QTcpSocket* s : socks)
    {
        recvBuffers_.remove(s);
        if (s)
        {
            s->disconnect();
            s->deleteLater();
        }
    }
    recvBuffers_.clear();
}

bool CimdrawWiringTcpJsonSource::isRunning() const
{
    return server_ && server_->isListening();
}

void CimdrawWiringTcpJsonSource::onNewConnection()
{
    if (!server_)
        return;
    while (server_->hasPendingConnections())
    {
        QTcpSocket* s = server_->nextPendingConnection();
        if (!s)
            continue;
        s->setParent(this);
        recvBuffers_.insert(s, QByteArray());
        connect(s, &QTcpSocket::readyRead, this, &CimdrawWiringTcpJsonSource::onSocketReadyRead);
        connect(s, &QTcpSocket::disconnected, this, &CimdrawWiringTcpJsonSource::onSocketDisconnected);
    }
}

void CimdrawWiringTcpJsonSource::onSocketDisconnected()
{
    auto* s = qobject_cast<QTcpSocket*>(sender());
    if (!s)
        return;
    recvBuffers_.remove(s);
    s->deleteLater();
}

void CimdrawWiringTcpJsonSource::onSocketReadyRead()
{
    auto* s = qobject_cast<QTcpSocket*>(sender());
    if (!s)
        return;
    appendAndProcessLines(s, s->readAll());
}

void CimdrawWiringTcpJsonSource::appendAndProcessLines(QTcpSocket* socket, const QByteArray& chunk)
{
    QByteArray& buf = recvBuffers_[socket];
    buf += chunk;

    CimdrawScene* scene = sceneResolver_ ? sceneResolver_() : nullptr;
    if (!scene)
    {
        while (buf.contains('\n'))
            buf.remove(0, buf.indexOf('\n') + 1);
        return;
    }

    for (;;)
    {
        const int nl = buf.indexOf('\n');
        if (nl < 0)
            break;
        QByteArray line = buf.left(nl);
        buf.remove(0, nl + 1);
        line = line.trimmed();
        if (line.isEmpty())
            continue;

        QJsonParseError err{};
        const QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject())
        {
            qWarning() << "CimdrawWiringTcpJsonSource: JSON parse error:" << err.errorString() << "line:" << line;
            continue;
        }
        applyJsonObjectToScene(scene, doc.object());
    }
}

void CimdrawWiringTcpJsonSource::applyJsonObjectToScene(CimdrawScene* scene, const QJsonObject& o)
{
    if (!scene)
        return;

    if (o.contains(QStringLiteral("batch")))
    {
        const QJsonValue bv = o.value(QStringLiteral("batch"));
        if (bv.isArray())
        {
            for (const QJsonValue& ev : bv.toArray())
            {
                if (ev.isObject())
                    applyJsonObjectToScene(scene, ev.toObject());
            }
        }
        return;
    }

    const QString key = o.value(QStringLiteral("key")).toString(o.value(QStringLiteral("k")).toString());
    if (key.isEmpty())
        return;

    QJsonValue jv;
    if (o.contains(QStringLiteral("value")))
        jv = o.value(QStringLiteral("value"));
    else if (o.contains(QStringLiteral("v")))
        jv = o.value(QStringLiteral("v"));
    else
        return;

    cimApplyWiringRuntimeValue(scene, key, jv.toVariant());
}
