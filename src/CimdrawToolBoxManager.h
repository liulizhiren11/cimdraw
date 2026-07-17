#ifndef CIMDRAWTOOLBOXMANAGER_H
#define CIMDRAWTOOLBOXMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>

class CimdrawImageManager;
class QListWidget;
class QWidget;
class QIcon;
class CimdrawFrame;
class QStackedWidget;
class QToolBox;

class CimdrawToolBoxManager : public QObject
{
    Q_OBJECT
public:
    explicit CimdrawToolBoxManager(CimdrawFrame* frame);
    ~CimdrawToolBoxManager();

    QWidget* getWidget();
    QToolBox* getToolBox(); // legacy compatibility only; main UI uses getWidget()
    QListWidget* activityBar() const;
    QListWidget* toolList(const QString& pageId) const;
    QStringList pageIds() const;
    QString pageTitle(const QString& pageId) const;
    void addSidebarPage(const QString& pageId, const QString& title, const QIcon& icon, QWidget* content);

    /** CIM 电力图元 */
    QListWidget* createPowerWiringToolListWidget();
    //基础图元
    QListWidget* createBaseToolListWidget();
    //图像图元
    QListWidget* createImageToolListWidget();
    /** 符号标准切换后刷新工具箱图标 */
    void refreshWiringToolIcons();

private:
    struct SidebarPage
    {
        QString id;
        QString title;
        QWidget* pageWidget = nullptr;
        QWidget* contentWidget = nullptr;
        QListWidget* toolList = nullptr;
    };

    QWidget* createSidebarPage(const QString& title, QWidget* content, const QString& pageId) const;

    QWidget* rootWidget_ = nullptr;
    QListWidget* activityBar_ = nullptr;
    QWidget* detailHost_ = nullptr;
    QStackedWidget* panelStack_ = nullptr;
    QList<SidebarPage> pages_;
    CimdrawImageManager* imageManager = nullptr;
    CimdrawFrame* frame = nullptr;
    QListWidget* powerWiringToolList_ = nullptr;
};

#endif
