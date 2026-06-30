#ifndef LZWIRINGTCPJSONSOURCE_H
#define LZWIRINGTCPJSONSOURCE_H

#include "LzIDataSource.h"
#include "LzSceneResolver.h"

#include <QHash>
#include <QHostAddress>
#include <QTcpServer>

class QTcpSocket;
class LzScene;
class QJsonObject;

/**
 * @brief 类型 id：wiring_tcp_json。监听 TCP，按行接收 JSON，写入 LzScene::setWiringData。
 *
 * 配置字段：listenAddress（默认 127.0.0.1）、listenPort（必填）。
 */
class LzWiringTcpJsonSource : public LzIDataSource
{
    Q_OBJECT
public:
    explicit LzWiringTcpJsonSource(LzSceneResolver resolver, QObject* parent = nullptr);
    ~LzWiringTcpJsonSource() override;

    QString typeId() const override { return QStringLiteral("wiring_tcp_json"); }
    bool configure(const QJsonObject& o) override;
    bool start() override;
    void stop() override;
    bool isRunning() const override;

    quint16 listenPort() const { return port_; }

private slots:
    void onNewConnection();
    void onSocketReadyRead();
    void onSocketDisconnected();

private:
    void appendAndProcessLines(QTcpSocket* socket, const QByteArray& chunk);
    static void applyJsonObjectToScene(LzScene* scene, const QJsonObject& o);

    LzSceneResolver sceneResolver_;
    QTcpServer* server_ = nullptr;
    QHostAddress bindAddress_{ QHostAddress::LocalHost };
    quint16 port_ = 0;
    QHash<QTcpSocket*, QByteArray> recvBuffers_;
};

#endif
