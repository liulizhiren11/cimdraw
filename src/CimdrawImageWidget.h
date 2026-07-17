#ifndef CIMDRAWIMAGEWIDGET_H
#define CIMDRAWIMAGEWIDGET_H

#include <QListWidget>
#include <QWidget>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QContextMenuEvent>

class CimdrawImageWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit CimdrawImageWidget(QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void deleteSelectItem(QListWidgetItem* item);
    void addImageItem(const QList<QString>& str);

private:
    void addImage();

    void deleteSelectedItem(QListWidgetItem* item);
};

#endif
