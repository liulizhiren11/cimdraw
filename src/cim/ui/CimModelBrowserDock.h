#ifndef CIMMODELBROWSERDOCK_H
#define CIMMODELBROWSERDOCK_H

#include "cim/model/CimModel.h"

#include <QDockWidget>
#include <QHash>
#include <QSet>

class QLineEdit;
class QTextEdit;
class QTreeWidget;
class QTreeWidgetItem;

class CimModelBrowserDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit CimModelBrowserDock(QWidget* parent = nullptr);

    void setImportResult(const CimModel& model,
                         const QStringList& loadedProfiles,
                         const QStringList& logs,
                         const QStringList& warnings,
                         const QStringList& errors,
                         const QSet<QString>& visualizedMrids);
    void setImportResult(const CimModel& model,
                         const QStringList& loadedProfiles,
                         const QStringList& logs,
                         const QStringList& warnings,
                         const QStringList& errors,
                         const QSet<QString>& visualizedMrids = {},
                         int generatedHelperLineCount = 0);
    void selectObjectByMrid(const QString& mrid);

signals:
    void objectActivated(const QString& mrid);

private slots:
    void onFilterTextChanged(const QString& text);
    void onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void onItemActivated(QTreeWidgetItem* item, int column);

private:
    void rebuildTree(const CimModel& model, const QSet<QString>& visualizedMrids);
    bool updateItemVisibility(QTreeWidgetItem* item, const QString& filterText);
    QString mridForItem(QTreeWidgetItem* item) const;

    QLineEdit* filterEdit_ = nullptr;
    QTreeWidget* treeWidget_ = nullptr;
    QTextEdit* summaryTextEdit_ = nullptr;
    QHash<QString, QTreeWidgetItem*> mridItemMap_;
    bool syncingSelection_ = false;
};

#endif // CIMMODELBROWSERDOCK_H
