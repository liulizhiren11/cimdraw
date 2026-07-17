#include <QApplication>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QDir>

#include "CimdrawView.h"
#include "CimdrawScene.h"
#include "CimdrawFile.h"
#include "tool/CimdrawConnectLineTool.h"
#include "tool/CimdrawTool.h"

class CimdrawViewPrivate
{
    Q_DECLARE_PUBLIC(CimdrawView)
public:
    CimdrawViewPrivate(CimdrawView* view)
        :q_ptr(view)
    {

    }

    QString fileName;
    QUndoStack* stack;
    QColor backGroundColor;
    QPixmap backgroundImage;
    QString imageFile;
    QPointF lastPos;

    CimdrawScene* scene;
    //用来判断文件保存状态的
    bool modified;
    CimdrawView* q_ptr;
};

CimdrawView::CimdrawView(QWidget* parent)
    :QGraphicsView(parent),d_ptr(new CimdrawViewPrivate(this))
{
    Q_D(CimdrawView);
    setMouseTracking(true);
    d->stack = new QUndoStack(this);
    connect(d->stack, &QUndoStack::canUndoChanged, this, [this](bool) {
        emit editChanged();
    });
    connect(d->stack, &QUndoStack::canRedoChanged, this, [this](bool) {
        emit editChanged();
    });
    connect(d->stack, &QUndoStack::indexChanged, this, [this](int) {
        emit editChanged();
    });
    connect(d->stack, &QUndoStack::cleanChanged, this, [this](bool clean) {
        if (clean)
        {
            d_ptr->modified = false;
        }
        emit editChanged();
    });
    d->scene = nullptr;
    d->backGroundColor = Qt::white;
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, Qt::white);
    setPalette(palette);
    d->modified = false;
    // 设置坐标原点在左上角
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setRenderHint(QPainter::Antialiasing);

    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏水平滚动条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // 隐藏垂直滚动条
}

CimdrawView::~CimdrawView()
{

}

CimdrawScene* CimdrawView::getScene()
{
    Q_D(const CimdrawView);
    return dynamic_cast<CimdrawScene*>(scene());
}

void CimdrawView::setPowerScene(CimdrawScene* pScene)
{
    Q_D(CimdrawView);
    d->scene = pScene;
    setScene(pScene);
}

void CimdrawView::setFileName(const QString& filename)
{
    Q_D(CimdrawView);
    d->fileName = filename;
}

void CimdrawView::setBackgroundCol(const QColor &c)
{
    Q_D(CimdrawView);
    d->backGroundColor = c;
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, c); // 设置背景色
    setPalette(palette);
}

QColor CimdrawView::getBackgroundCol()
{
    Q_D(const CimdrawView);
    return d->backGroundColor;
}

void CimdrawView::setBackgroundImage(const QString& image)
{
    d_ptr->imageFile = image;
    this->viewport()->update();
}

QString CimdrawView::getBackgroundImage()
{
    Q_D(CimdrawView);
    return d->imageFile;
}

void CimdrawView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
}

void CimdrawView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);
}

void CimdrawView::paintEvent(QPaintEvent* event)
{
    QPainter painter(this->viewport());

    QString imagePath = d_ptr->imageFile;
    if (!imagePath.isEmpty())
    {
        QPixmap pixmap(imagePath);
        if (!pixmap.isNull())
        {
            painter.drawPixmap(0, 0, this->viewport()->width(), this->viewport()->height(), pixmap);
        }
    }

    // 继续执行其他绘制工作
    QGraphicsView::paintEvent(event);
}

void CimdrawView::mouseMoveEvent(QMouseEvent * event)
{
    Q_D(CimdrawView);
    QPointF pos = mapToScene(event->pos());
    emit mouseMove(pos);
    CimdrawScene* currentScene = getScene();
    const bool transformActive =
        currentScene && currentScene->isInteractiveTransformActive();
    if (!(event->buttons() & Qt::LeftButton))
    {
        if (auto* tool = qobject_cast<CimdrawConnectLineTool*>(CimdrawToolManager::getInstance()->getCurrentTool()))
            tool->abortCurrentWire(getScene(), false);
    }
    if (!transformActive
        && (event->buttons() & Qt::LeftButton)
        && (event->modifiers() & Qt::AltModifier))
    {
        QPointF delta = event->pos() - d->lastPos;

        QRectF sceneRect = this->sceneRect();
        sceneRect.translate(-delta);
        this->setSceneRect(sceneRect);

        ensureVisible(sceneRect);

        d->lastPos = event->pos();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void CimdrawView::leaveEvent(QEvent* event)
{
    QGraphicsView::leaveEvent(event);
    if (QApplication::mouseButtons() & Qt::LeftButton)
    {
        if (auto* tool = qobject_cast<CimdrawConnectLineTool*>(CimdrawToolManager::getInstance()->getCurrentTool()))
            tool->abortCurrentWire(getScene());
    }
}

void CimdrawView::focusOutEvent(QFocusEvent* event)
{
    QGraphicsView::focusOutEvent(event);
    if (auto* tool = qobject_cast<CimdrawConnectLineTool*>(CimdrawToolManager::getInstance()->getCurrentTool()))
        tool->abortCurrentWire(getScene());
}

bool CimdrawView::viewportEvent(QEvent* event)
{
    if (event && event->type() == QEvent::Leave)
    {
        if (auto* tool = qobject_cast<CimdrawConnectLineTool*>(CimdrawToolManager::getInstance()->getCurrentTool()))
            tool->abortCurrentWire(getScene());
    }
    return QGraphicsView::viewportEvent(event);
}

void CimdrawView::mousePressEvent(QMouseEvent * event)
{
    Q_D(CimdrawView);
    CimdrawScene* currentScene = getScene();
    const bool transformActive =
        currentScene && currentScene->isInteractiveTransformActive();
    if (!transformActive
        && event->button() == Qt::LeftButton
        && (event->modifiers() & Qt::AltModifier))
    {
        d->lastPos = event->pos();
    }
    QGraphicsView::mousePressEvent(event);
}

void CimdrawView::mouseReleaseEvent(QMouseEvent * event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void CimdrawView::wheelEvent(QWheelEvent* event)
{
    // 获取鼠标在视口中的位置
    QPointF cursorPoint = event->position();
    // 转换为场景坐标
    QPointF scenePos = mapToScene(cursorPoint.toPoint());
    
    // 获取视口尺寸
    qreal viewWidth = viewport()->width();
    qreal viewHeight = viewport()->height();
    
    // 计算鼠标位置相对于视口的比例
    qreal hscale = cursorPoint.x() / viewWidth;
    qreal vscale = cursorPoint.y() / viewHeight;
    
    // 获取滚轮角度增量
    QPointF angleDelta = event->angleDelta();
    
    // 计算缩放因子
    const qreal scaleFactor = 1.15; // 缩放因子
    qreal zoomFactor = 1.0;
    
    // 判断是垂直滚动还是水平滚动
    if (qAbs(angleDelta.y()) > qAbs(angleDelta.x())) {
        // 垂直滚动
        zoomFactor = (angleDelta.y() > 0) ? scaleFactor : 1.0 / scaleFactor;
    } else {
        // 水平滚动
        zoomFactor = (angleDelta.x() > 0) ? scaleFactor : 1.0 / scaleFactor;
    }
    
    // 执行缩放
    scale(zoomFactor, zoomFactor);
    
    // 保持鼠标位置不变
    QPointF newScenePos = mapToScene(cursorPoint.toPoint());
    QPointF delta = scenePos - newScenePos;
    translate(delta.x(), delta.y());
}

void CimdrawView::zoomOut()
{
    scale(1 / 1.25, 1 / 1.25);
}

void CimdrawView::zoomIn()
{
    scale(1.25, 1.25);
}

void CimdrawView::zoomReset()
{
    resetTransform();
}

void CimdrawView::fitSceneToView(const QRectF& rect, Qt::AspectRatioMode aspectRatioMode)
{
    if (rect.isNull() || rect.isEmpty())
        return;
    fitInView(rect, aspectRatioMode);
}

void CimdrawView::closeEvent(QCloseEvent * pEvent)
{
    Q_D(CimdrawView);
    if (d->scene && isModified())
    {
        QString file_name;
        if (d->fileName.isEmpty())
        {
            file_name = "unknown";
        }
        else
        {
            file_name = d->fileName.section("/", -1, -1);
        }
        QString text = QString("文件：%1 已有更改，是否保存？").arg(file_name);
        int ret = QMessageBox::warning(this, QObject::tr("保存更改"), text, QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
        switch (ret)
        {
        case QMessageBox::Yes:
        {
            QString str;
            if (!save(str) && str.isEmpty())
            {
                pEvent->ignore();
                return;
            }

            pEvent->accept();
        }
        break;
        case QMessageBox::No:
            pEvent->accept();
            break;
        case QMessageBox::Cancel:
            pEvent->ignore();
            return;
        }
    }

    if (d->scene)
    {
        setScene(nullptr);
        d->scene->clear();
        delete d->scene;
        d->scene = nullptr;
    }

    deleteLater();
}


QUndoStack* CimdrawView::getStack()
{
    Q_D(const CimdrawView);
    return d->stack;
}

void CimdrawView::editUndo()
{
    Q_D(CimdrawView);
    if (d->stack->canUndo())
    {
        d->stack->undo();
    }
    emit editChanged();
}

void CimdrawView::editRedo()
{
    Q_D(CimdrawView);
    if (d->stack->canRedo())
    {
        d->stack->redo();
    }
    emit editChanged();
}

bool CimdrawView::save(QString& filename)
{
    Q_D(CimdrawView);
    if (d->fileName.isEmpty())
    {
        QString filePathName = QString("%1/../../%2").arg(qApp->applicationDirPath()).arg(FILE_PATH);
        QString saveFileName = QFileDialog::getSaveFileName(this, QObject::tr("保存文件"), filePathName,
            QObject::tr("Cimdraw 场景 (*.xml);;PDW (*.pdw)"));
        if (saveFileName.isEmpty())
        {
            return false;
        }

        d->fileName = saveFileName;
    }

    if (save())
    {
        filename = QFileInfo(d->fileName).fileName();
        return true;
    }
    return false;
}

bool CimdrawView::save()
{
    Q_D(CimdrawView);
    CimdrawFile powerfile(d->fileName);
    const bool ok = powerfile.save(d->scene);
    if (ok)
    {
        setModified(false);
    }
    return ok;
}

void CimdrawView::setModified(bool value)
{
    Q_D(CimdrawView);
    d->modified = value;
    if (!value && d->stack)
    {
        d->stack->setClean();
    }
}

bool CimdrawView::isModified()
{
    Q_D(const CimdrawView);
    return d->modified || (d->stack && !d->stack->isClean());
}

bool CimdrawView::navigateTo(const QString& filename, bool addToList, QString* inout_error_msg)
{
    return true;
}
