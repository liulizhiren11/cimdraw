#include "FrameWorkspaceBuilder.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPalette>
#include <QPushButton>
#include <QStatusBar>
#include <QToolBar>
#include <QWidget>

#include "CimdrawAppConfig.h"
#include "wiring/base/CimdrawWiringSymbolStyle.h"

FrameWorkspaceBuilder::CanvasToolbarResult FrameWorkspaceBuilder::createCanvasToolbar(QMainWindow* host) const
{
    CanvasToolbarResult result;
    if (!host)
        return result;

    auto* container = new QWidget(host);
    auto* layout = new QHBoxLayout(container);
    layout->setSpacing(10);
    layout->setContentsMargins(5, 2, 5, 2);

    auto* docBgLabel = new QLabel(QObject::tr("画布背景色:"), container);
    result.backgroundColorButton = new QPushButton(host);
    result.backgroundColorButton->setAutoFillBackground(true);
    result.backgroundColorButton->setFlat(true);
    result.backgroundColorButton->setAutoDefault(false);
    result.backgroundColorButton->setFixedWidth(80);
    QPalette palette = result.backgroundColorButton->palette();
    palette.setColor(QPalette::Button, QColor(Qt::white));
    result.backgroundColorButton->setPalette(palette);

    result.backgroundImageButton = new QPushButton(QObject::tr("画布背景图片..."), host);

    layout->addWidget(docBgLabel, 0, Qt::AlignVCenter);
    layout->addWidget(result.backgroundColorButton, 0, Qt::AlignVCenter);
    layout->addSpacing(3);
    layout->addWidget(result.backgroundImageButton, 0, Qt::AlignVCenter);

    result.toolBar = host->addToolBar(QObject::tr("Canvas"));
    result.toolBar->addWidget(container);
    return result;
}

FrameWorkspaceBuilder::WiringToolbarResult FrameWorkspaceBuilder::createWiringToolbar(QMainWindow* host) const
{
    WiringToolbarResult result;
    if (!host)
        return result;

    auto* container = new QWidget(host);
    auto* layout = new QHBoxLayout(container);
    layout->setContentsMargins(4, 0, 4, 0);
    layout->setSpacing(6);

    auto* label = new QLabel(QObject::tr("接线符号:"), container);
    result.standardCombo = new QComboBox(container);
    result.standardCombo->setMinimumWidth(120);

    const struct { CimdrawWiringSymbolStandard value; } options[] = {
        {CimdrawWiringSymbolStandard::GB},
        {CimdrawWiringSymbolStandard::IEC},
        {CimdrawWiringSymbolStandard::ANSI},
    };
    for (const auto& opt : options)
    {
        result.standardCombo->addItem(CimdrawAppConfig::symbolStandardDisplayName(opt.value),
                                      static_cast<int>(opt.value));
    }

    layout->addWidget(label);
    layout->addWidget(result.standardCombo);

    result.toolBar = host->addToolBar(QObject::tr("Wiring"));
    result.toolBar->setObjectName(QStringLiteral("WiringSymbolStandardBar"));
    result.toolBar->setMovable(true);
    result.toolBar->addWidget(container);
    return result;
}

FrameWorkspaceBuilder::StatusBarResult FrameWorkspaceBuilder::createStatusBar(QMainWindow* host) const
{
    StatusBarResult result;
    if (!host)
        return result;

    result.xLineEdit = new QLineEdit(QString(), host);
    result.xLineEdit->setObjectName(QStringLiteral("statusCoordEdit"));
    result.xLineEdit->setReadOnly(true);
    result.xLineEdit->setFixedWidth(96);
    result.xLineEdit->setFrame(false);

    result.yLineEdit = new QLineEdit(QString(), host);
    result.yLineEdit->setObjectName(QStringLiteral("statusCoordEdit"));
    result.yLineEdit->setReadOnly(true);
    result.yLineEdit->setFixedWidth(96);
    result.yLineEdit->setFrame(false);

    result.messageLineEdit = new QLineEdit(QString(), host);
    result.messageLineEdit->setObjectName(QStringLiteral("statusMessageEdit"));
    result.messageLineEdit->setReadOnly(true);
    result.messageLineEdit->setMinimumWidth(260);
    result.messageLineEdit->setFrame(false);

    result.statusBar = new QStatusBar(host);
    result.statusBar->setSizeGripEnabled(false);
    result.statusBar->addWidget(result.xLineEdit);
    result.statusBar->addWidget(result.yLineEdit);
    result.statusBar->addPermanentWidget(result.messageLineEdit, 1);
    host->setStatusBar(result.statusBar);
    return result;
}
