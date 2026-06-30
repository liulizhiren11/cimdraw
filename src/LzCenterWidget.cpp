#include <QTabWidget>
#include <QVBoxLayout>
#include <QToolButton>
#include <QTabBar>
#include <QDebug>

#include "LzCenterWidget.h"
#include "LzView.h"

LzCenterWidget::LzCenterWidget()
    :d_ptr(new LzCenterWidgetPrivate(this))
{
    setObjectName(QStringLiteral("editorHost"));
    d_ptr->vboxLayout = new QVBoxLayout(this);
    d_ptr->vboxLayout->setContentsMargins(0, 0, 0, 0);
    d_ptr->vboxLayout->setSpacing(0);

    d_ptr->tabWidget = new QTabWidget(this);
    d_ptr->tabWidget->setObjectName(QStringLiteral("editorTabs"));
    d_ptr->tabWidget->setContentsMargins(0, 0, 0, 0);
    d_ptr->tabWidget->setTabsClosable(true);
    d_ptr->tabWidget->setDocumentMode(true);
    d_ptr->tabWidget->setMovable(true);
    d_ptr->tabWidget->tabBar()->setExpanding(false);
    d_ptr->tabWidget->tabBar()->setDrawBase(false);
    connect(d_ptr->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    d_ptr->vboxLayout->addWidget(d_ptr->tabWidget);

    QToolButton* m_newTabButton = new QToolButton(this);
    m_newTabButton->setObjectName(QStringLiteral("editorTabCornerButton"));
    m_newTabButton->setAutoRaise(true);
    m_newTabButton->setToolTip(tr("Add new page"));
    m_newTabButton->setIcon(QIcon(":/image/tab_add.png"));

    d_ptr->tabWidget->setCornerWidget(m_newTabButton, Qt::TopLeftCorner);
    connect(m_newTabButton, SIGNAL(clicked()), this, SLOT(newTab()));

    QToolButton* m_closeTabButton = new QToolButton(this);
    m_closeTabButton->setObjectName(QStringLiteral("editorTabCornerButton"));
    m_closeTabButton->setEnabled(true);
    m_closeTabButton->setAutoRaise(true);
    m_closeTabButton->setToolTip(tr("Close current page"));
    m_closeTabButton->setIcon(QIcon(":/image/tab_close.png"));

    d_ptr->tabWidget->setCornerWidget(m_closeTabButton, Qt::TopRightCorner);
    connect(m_closeTabButton, SIGNAL(clicked()), this, SLOT(closeTab()));
}

LzCenterWidget::~LzCenterWidget()
{

}

void LzCenterWidget::setBackgroundColor(QWidget* w, QColor &c)
{
    if (!w)
        return;
        
    QPalette pal = w->palette();
    pal.setColor(QPalette::Window, c);
    w->setAutoFillBackground(true);
    w->setPalette(pal);
}

LzView* LzCenterWidget::getCurrentView() const
{
    Q_D(const LzCenterWidget);
    if(d->tabWidget)
    {
        return dynamic_cast<LzView*>(d->tabWidget->currentWidget());
    }
    return nullptr;
}

QTabWidget* LzCenterWidget::getTabWidget()
{
    Q_D(const LzCenterWidget);
    return d->tabWidget;
}

QGraphicsView *LzCenterWidget::newTab(QGraphicsView* pView)
{
    Q_D(LzCenterWidget);
    if (!pView)
    {
        return nullptr;
    }
    d->tabWidget->addTab(pView, tr("unknown"));
    d->tabWidget->setCurrentWidget(pView);
    return pView;
}

void LzCenterWidget::newTab()
{
    emit requestNewScene();
}

void LzCenterWidget::closeTab()
{
    Q_D(LzCenterWidget);
    LzView* pView = getCurrentView();
    if (!pView)
    {
        return;
    }

    if (pView->close())
    {
        d->tabWidget->removeTab(d->tabWidget->indexOf(pView));
        if (d->tabWidget->count() == 0)
        {
            emit closeAll();
        }

        emit currentTabClosed();
    }
}

void LzCenterWidget::closeTab(int index)
{
    Q_D(LzCenterWidget);
    if (!d->tabWidget)
    {
        return;
    }

    if (index < 0 || index >= d->tabWidget->count())
    {
        return;
    }

    LzView* childWidget = dynamic_cast<LzView*>(d->tabWidget->widget(index));
    if (!childWidget)
    {
        return;
    }

    if (childWidget->close())
    {
        d->tabWidget->removeTab(index);
        if (d->tabWidget->count() == 0)
        {
            emit closeAll();
        }

        emit currentTabClosed();
    }
}
