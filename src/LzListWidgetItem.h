#ifndef LZLISTWIDGETITEM_H
#define LZLISTWIDGETITEM_H

#include <QListWidgetItem>
#include <QVariant>
#include <QScopedPointer>
#include "LzItemConfig.h"

class LzTool;
class LzListWidgetItemPrivate;

class LzListWidgetItem : public QListWidgetItem
{
public:
    LzListWidgetItem(const QSize&size, LzTool* tool, const QVariant& data = QVariant(), QListWidget *view = nullptr);
    LzTool* getTool();
    const QVariant& getData();
    QString getText() const;

    CREATE_MODE getCreateMode();
private:
    Q_DECLARE_PRIVATE(LzListWidgetItem);
    QScopedPointer<LzListWidgetItemPrivate> d_ptr;
};
#endif
