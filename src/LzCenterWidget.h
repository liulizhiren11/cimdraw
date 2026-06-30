#ifndef LZCENTERWIDGET_H
#define LZCENTERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QScopedPointer>

class CPowerView;
class QTabWidget;
class LzCenterWidgetPrivate;
class LzView;

class LzCenterWidget :public QWidget
{
    Q_OBJECT
public:
    LzCenterWidget();
    ~LzCenterWidget();

    void setBackgroundColor(QWidget*, QColor &c);

    LzView* getCurrentView() const;

    QTabWidget * getTabWidget();
signals:
    void closeAll();
    void requestNewScene();
    void currentTabClosed();

public slots:
    QGraphicsView *newTab(QGraphicsView* pView);

    void newTab();

    void closeTab();

    void closeTab(int index);
private:
    Q_DECLARE_PRIVATE(LzCenterWidget);
    QScopedPointer<LzCenterWidgetPrivate> d_ptr;
};

class LzCenterWidgetPrivate
{
public:
    LzCenterWidgetPrivate(LzCenterWidget* widget)
        :q_ptr(widget),vboxLayout(nullptr),tabWidget(nullptr)
    {

    }
    QVBoxLayout* vboxLayout;
    QTabWidget* tabWidget;
    LzCenterWidget* q_ptr;
};
#endif
