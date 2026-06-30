#ifndef LZVIEW_H
#define LZVIEW_H

#include <QGraphicsView>
#include <QUndoStack>
#include <QImage>
#include <QPixmap>
#include <QPointF>
#include <QList>
#include <QMap>
#include <QPair>
#include <QPalette>
#include <QToolTip>
#include <QGraphicsSceneMouseEvent>
#include <QScopedPointer>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

class LzScene;
class LzViewPrivate;

class LzView : public QGraphicsView
{
    Q_OBJECT
public:
    LzView(QWidget* parent = nullptr);

    ~LzView();

    virtual LzScene* getScene();

    virtual void setPowerScene(LzScene* scene);

    void setFileName(const QString& filename);

    void setBackgroundCol(const QColor &c);

    QColor getBackgroundCol();

    void setBackgroundImage(const QString& iamge);

    QString getBackgroundImage();

    QUndoStack* getStack();

    void editUndo();
    void editRedo();

    bool save(QString& filename);

    bool save();
    void setModified(bool value);
    bool isModified();

    void zoomOut();
    void zoomIn();
    void zoomReset();
    void fitSceneToView(const QRectF& rect, Qt::AspectRatioMode aspectRatioMode = Qt::KeepAspectRatio);

    //链接
    virtual bool navigateTo(const QString& hyperlink, bool addToList = true, QString* inout_error_msg = 0);
signals:
    void editChanged();
    void mouseMove(const QPointF& point);

protected:
    virtual void mouseMoveEvent(QMouseEvent * event);
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void leaveEvent(QEvent* event) override;
    virtual void focusOutEvent(QFocusEvent* event) override;
    virtual bool viewportEvent(QEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event);
    virtual void closeEvent(QCloseEvent * pEvent);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent* event);
protected:
    Q_DECLARE_PRIVATE(LzView);
    QScopedPointer<LzViewPrivate> d_ptr;

};

#endif
