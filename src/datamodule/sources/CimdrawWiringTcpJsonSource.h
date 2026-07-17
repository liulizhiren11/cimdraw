#ifndef CIMDRAWWIRINGTCPJSONSOURCE_H
#define CIMDRAWWIRINGTCPJSONSOURCE_H

#include "CimdrawIDataSource.h"
#include "CimdrawSceneResolver.h"

#include <QHash>
#include <QHostAddress>
#include <QTcpServer>

class QTcpSocket;
class CimdrawScene;
class QJsonObject;

/**
 * @brief 类型 id：wiring_tcp_json。监听 TCP，按行接收 JSON，写入 CimdrawScene::setWiringData。
 *
 * 配置字段：listenAddress（默认 127.0.0.1）、listenPort（必填）。
 */
class CimdrawWiringTcpJsonSource : public CimdrawIDataSource
{
    Q_OBJECT
public:
    explicit CimdrawWiringTcpJsonSource(CimdrawSceneResolver resolver, QObject* parent = nullptr);
    ~CimdrawWiringTcpJsonSource() override;

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
    static void applyJsonObjectToScene(CimdrawScene* scene, const QJsonObject& o);

    CimdrawSceneResolver sceneResolver_;
    QTcpServer* server_ = nullptr;
    QHostAddress bindAddress_{ QHostAddress::LocalHost };
    quint16 port_ = 0;
    QHash<QTcpSocket*, QByteArray> recvBuffers_;
};

#endif
