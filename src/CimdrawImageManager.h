#ifndef CIMDRAWIMAGEMANAGER_H
#define CIMDRAWIMAGEMANAGER_H

#include <QObject>
#include <QScopedPointer>

class CimdrawImageManagerPrivate;
class QListWidget;
class CimdrawImageTool;
class CimdrawImageWidget;

class CimdrawImageManager : public QObject
{
    Q_OBJECT
public:
    CimdrawImageManager();
    ~CimdrawImageManager();

    QListWidget* getWidget();

public slots:
    void onAddImageItem(const QList<QString>& fileName);
private:
    Q_DECLARE_PRIVATE(CimdrawImageManager);
    QScopedPointer<CimdrawImageManagerPrivate> d_ptr;
};

class CimdrawImageManagerPrivate
{
    Q_DECLARE_PUBLIC(CimdrawImageManager)
public:
    explicit CimdrawImageManagerPrivate(CimdrawImageManager* manager)
        : q_ptr(manager)
    {
    }

    CimdrawImageTool* tool = nullptr;
    CimdrawImageWidget* widget = nullptr;
    CimdrawImageManager* q_ptr = nullptr;
};

#endif
