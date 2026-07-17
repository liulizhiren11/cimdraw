#ifndef CIMDRAWLISTWIDGETITEM_H
#define CIMDRAWLISTWIDGETITEM_H

#include <QListWidgetItem>
#include <QVariant>
#include <QScopedPointer>
#include "CimdrawItemConfig.h"

class CimdrawTool;
class CimdrawListWidgetItemPrivate;

class CimdrawListWidgetItem : public QListWidgetItem
{
public:
    CimdrawListWidgetItem(const QSize&size, CimdrawTool* tool, const QVariant& data = QVariant(), QListWidget *view = nullptr);
    CimdrawTool* getTool();
    const QVariant& getData();
    QString getText() const;

    CREATE_MODE getCreateMode();
private:
    Q_DECLARE_PRIVATE(CimdrawListWidgetItem);
    QScopedPointer<CimdrawListWidgetItemPrivate> d_ptr;
};
#endif
