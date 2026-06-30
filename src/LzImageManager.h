#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QObject>
#include <QScopedPointer>

class LzImageManagerPrivate;
class QListWidget;
class LzImageTool;
class LzImageWidget;

class LzImageManager : public QObject
{
    Q_OBJECT
public:
    LzImageManager();
    ~LzImageManager();

    QListWidget* getWidget();

public slots:
    void onAddImageItem(const QList<QString>& fileName);
private:
    Q_DECLARE_PRIVATE(LzImageManager);
    QScopedPointer<LzImageManagerPrivate> d_ptr;
};

class LzImageManagerPrivate
{
    Q_DECLARE_PUBLIC(LzImageManager)
public:
    LzImageManagerPrivate(LzImageManager* manager)
        :q_ptr(manager)
    {

    }
    LzImageTool* tool;
    LzImageWidget* widget;
    LzImageManager* q_ptr;
};

#endif
