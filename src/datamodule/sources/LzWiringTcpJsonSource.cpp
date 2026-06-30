#include "LzWiringTcpJsonSource.h"
#include "LzScene.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QTcpSocket>

#include <QDebug>

LzWiringTcpJsonSource::LzWiringTcpJsonSource(LzSceneResolver resolver, QObject* parent)
    : LzIDataSource(parent)
    , sceneResolver_(std::move(resolver))
{
    server_ = new QTcpServer(this);
    connect(server_, &QTcpServer::newConnection, this, &LzWiringTcpJsonSource::onNewConnection);
}

LzWiringTcpJsonSource::~LzWiringTcpJsonSource()
{
    stop();
}

bool LzWiringTcpJsonSource::configure(const QJsonObject& o)
{
    const QJsonValue p = o.contains(QStringLiteral("listenPort")) ? o.value(QStringLiteral("listenPort"))
                                                                  : o.value(QStringLiteral("port"));
    const int raw = p.toInt(0);
    if (raw <= 0 || raw > 65535)
    {
        qWarning() << "LzWiringTcpJsonSource: invalid listenPort";
        return false;
    }
    port_ = static_cast<quint16>(raw);

    const QString hostStr = o.value(QStringLiteral("listenAddress")).toString(
        o.value(QStringLiteral("bindAddress")).toString(QStringLiteral("127.0.0.1")));
    const QHostAddress addr(hostStr);
    if (addr.isNull())
    {
        qWarning() << "LzWiringTcpJsonSource: invalid listenAddress" << hostStr;
        return false;
    }
    bindAddress_ = addr;
    return true;
}

bool LzWiringTcpJsonSource::start()
{
    if (!server_ || port_ == 0)
        return false;
    if (server_->isListening())
        return true;
    if (!server_->listen(bindAddress_, port_))
    {
        qWarning() << "LzWiringTcpJsonSource: listen failed" << bindAddress_.toString() << port_
                   << server_->errorString();
        return false;
    }
    qInfo() << "LzWiringTcpJsonSource: listening" << server_->serverAddress().toString() << server_->serverPort();
    return true;
}

void LzWiringTcpJsonSource::stop()
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

bool LzWiringTcpJsonSource::isRunning() const
{
    return server_ && server_->isListening();
}

void LzWiringTcpJsonSource::onNewConnection()
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
        connect(s, &QTcpSocket::readyRead, this, &LzWiringTcpJsonSource::onSocketReadyRead);
        connect(s, &QTcpSocket::disconnected, this, &LzWiringTcpJsonSource::onSocketDisconnected);
    }
}

void LzWiringTcpJsonSource::onSocketDisconnected()
{
    auto* s = qobject_cast<QTcpSocket*>(sender());
    if (!s)
        return;
    recvBuffers_.remove(s);
    s->deleteLater();
}

void LzWiringTcpJsonSource::onSocketReadyRead()
{
    auto* s = qobject_cast<QTcpSocket*>(sender());
    if (!s)
        return;
    appendAndProcessLines(s, s->readAll());
}

void LzWiringTcpJsonSource::appendAndProcessLines(QTcpSocket* socket, const QByteArray& chunk)
{
    QByteArray& buf = recvBuffers_[socket];
    buf += chunk;

    LzScene* scene = sceneResolver_ ? sceneResolver_() : nullptr;
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
            qWarning() << "LzWiringTcpJsonSource: JSON parse error:" << err.errorString() << "line:" << line;
            continue;
        }
        applyJsonObjectToScene(scene, doc.object());
    }
}

void LzWiringTcpJsonSource::applyJsonObjectToScene(LzScene* scene, const QJsonObject& o)
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

    scene->setWiringData(key, jv.toVariant());
}
