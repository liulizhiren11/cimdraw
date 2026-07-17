#ifndef CIMDRAWCENTERWIDGET_H
#define CIMDRAWCENTERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QScopedPointer>

class CPowerView;
class QTabWidget;
class CimdrawCenterWidgetPrivate;
class CimdrawView;

class CimdrawCenterWidget : public QWidget
{
    Q_OBJECT
public:
    CimdrawCenterWidget();
    ~CimdrawCenterWidget();

    void setBackgroundColor(QWidget*, QColor& c);

    CimdrawView* getCurrentView() const;

    QTabWidget* getTabWidget();
signals:
    void closeAll();
    void requestNewScene();
    void currentTabClosed();

public slots:
    QGraphicsView* newTab(QGraphicsView* pView);

    void newTab();

    void closeTab();

    void closeTab(int index);
private:
    Q_DECLARE_PRIVATE(CimdrawCenterWidget);
    QScopedPointer<CimdrawCenterWidgetPrivate> d_ptr;
};

class CimdrawCenterWidgetPrivate
{
public:
    explicit CimdrawCenterWidgetPrivate(CimdrawCenterWidget* widget)
        : q_ptr(widget), vboxLayout(nullptr), tabWidget(nullptr)
    {
    }
    QVBoxLayout* vboxLayout;
    QTabWidget* tabWidget;
    CimdrawCenterWidget* q_ptr;
};
#endif
