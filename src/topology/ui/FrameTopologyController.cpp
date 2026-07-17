#include "FrameTopologyController.h"

#include <QColor>
#include <QMessageBox>

#include "CimdrawScene.h"
#include "CimdrawView.h"

namespace {

QString topologyTabTitle(FrameTopologyController::GeneratedMode mode)
{
    switch (mode)
    {
    case FrameTopologyController::GeneratedMode::PowerSystem:
        return QObject::tr("电力拓扑图");
    case FrameTopologyController::GeneratedMode::Combined:
    default:
        return QObject::tr("关系拓扑图");
    }
}

} // namespace

void FrameTopologyController::showSelectedSummary(QWidget* host, CimdrawScene* scene) const
{
    if (!scene)
    {
        QMessageBox::information(host, QObject::tr("拓扑摘要"), QObject::tr("当前场景不可用。"));
        return;
    }

    if (scene->getSelections().count() != 1)
    {
        QMessageBox::information(host, QObject::tr("拓扑摘要"), QObject::tr("请先只选中一个图元。"));
        return;
    }

    const QStringList lines = scene->selectedTopologySummaryLines();
    if (lines.isEmpty())
    {
        QMessageBox::information(host, QObject::tr("拓扑摘要"), QObject::tr("当前选中对象不是可识别的拓扑图元。"));
        return;
    }
    QMessageBox::information(host, QObject::tr("拓扑摘要"), lines.join('\n'));
}

void FrameTopologyController::highlightSelected(QWidget* host, CimdrawScene* scene) const
{
    if (!scene)
        return;

    if (scene->getSelections().count() != 1)
    {
        QMessageBox::information(host, QObject::tr("拓扑高亮"), QObject::tr("请先只选中一个图元。"));
        return;
    }

    scene->clearTopologyHighlights();
    const CimTopologyHighlightSummary highlightSummary = scene->selectedTopologyHighlightSummary();
    if (!highlightSummary.canHighlight)
    {
        QMessageBox::information(host, QObject::tr("拓扑高亮"), QObject::tr("当前选中图元未参与拓扑分析。"));
        return;
    }
    if (!highlightSummary.isValid())
        return;

    scene->highlightTopologyEdges(highlightSummary.edgeIds, true, QColor(220, 80, 60));
    for (const QString& nodeId : highlightSummary.nodeIds)
        scene->highlightTopologyNode(nodeId, true, QColor(255, 208, 0));
}

void FrameTopologyController::clearHighlights(CimdrawScene* scene) const
{
    if (scene)
        scene->clearTopologyHighlights();
}

void FrameTopologyController::generateGraph(const GenerateContext& context, GeneratedMode mode) const
{
    if (!context.sourceScene)
    {
        QMessageBox::information(context.host, QObject::tr("生成拓扑图"), QObject::tr("当前场景不可用。"));
        return;
    }

    CimdrawScene* sourceScene = context.sourceScene;
    sourceScene->rebuildTopologyIndex();
    const CimdrawTopologyDomain domainFilter = mode == GeneratedMode::PowerSystem
        ? CimdrawTopologyDomain::PowerSystem
        : CimdrawTopologyDomain::None;
    const CimGeneratedTopologyGraphSummary graphSummary =
        sourceScene->generatedTopologyGraphSummary(domainFilter);

    if (graphSummary.isEmpty())
    {
        const QString message = mode == GeneratedMode::PowerSystem
            ? QObject::tr("当前场景没有可生成的电力拓扑设备。")
            : QObject::tr("当前场景没有找到可参与拓扑的图元。");
        QMessageBox::information(context.host, QObject::tr("生成拓扑图"), message);
        return;
    }

    if (!context.createTargetView)
        return;

    CimdrawView* targetView = context.createTargetView();
    if (!targetView)
        return;
    if (context.renameCurrentTab)
        context.renameCurrentTab(topologyTabTitle(mode));

    CimdrawScene* targetScene = targetView->getScene();
    if (!targetScene)
        return;

    const CimGeneratedTopologySceneBuildResult buildResult =
        targetScene->renderGeneratedTopologyGraphSummary(graphSummary);
    targetScene->rebuildTopologyIndex();
    targetView->setModified(true);
    if (context.onGenerated)
        context.onGenerated();

    if (buildResult.generatedLineCount <= 0)
    {
        QMessageBox::information(context.host,
                                 QObject::tr("生成拓扑图"),
                                 QObject::tr("已生成拓扑节点，但当前没有找到可连接的拓扑关系线。"));
    }
}
