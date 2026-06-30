#ifndef LZEDITCONNECTLINEPATHCOMMAND_H
#define LZEDITCONNECTLINEPATHCOMMAND_H

#include <QUndoCommand>
#include <QPointer>
#include <QVector>
#include <QPointF>

#include "LzConnectConfig.h"

class LzConnectLine;
class LzScene;
class LzView;
class QObject;

/** 连接线路径编辑撤销/还原（折点拖曳、插点、清空重规划等） */
class LzEditConnectLinePathCommand : public QUndoCommand
{
public:
    struct State
    {
        QVector<QPointF> scenePoints;
        ConnectorPathRoutingMode routingMode = ConnectorPathRoutingMode::Auto;
        QPointer<QObject> startItemObject;
        QPointer<QObject> endItemObject;
        int startPortOrdinal = -1;
        int endPortOrdinal = -1;
    };

    LzEditConnectLinePathCommand(LzConnectLine* line,
                                 const State& beforeState,
                                 const State& afterState,
                                 QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

    static State captureState(LzConnectLine* line);

    /** 路径、路由模式或端点绑定有实质变化时入栈 */
    static void pushIfChanged(LzView* view,
                              LzConnectLine* line,
                              const State& beforeState,
                              const State& afterState,
                              const QString& actionText = QString());

    int id() const override;
    bool mergeWith(const QUndoCommand* other) override;

private:
    void applyState(const State& state);

    QPointer<LzConnectLine> m_line;
    State m_beforeState;
    State m_afterState;
};

#endif
