#ifndef LZMIMEDATA_H_
#define LZMIMEDATA_H_

#include <QMimeData>
#include <QGraphicsItem>
#include <QList>
#include <QScopedPointer>
#include <QPointF>

class LzMimeDataPrivate;

class LzMimeData : public  QMimeData
{
	Q_OBJECT
public:
	LzMimeData(QList<QGraphicsItem* > items);
	~LzMimeData();
	QList<QGraphicsItem*> items() const;
	QList<QPointF> delta() const;

    /** 生成一次粘贴用的新图元（不修改剪贴板模板），平移 delta，并重绑连线端点 */
    QList<QGraphicsItem*> instantiatePasteBatch(const QPointF& deltaScene) const;
private:
    Q_DECLARE_PRIVATE(LzMimeData);
    QScopedPointer<LzMimeDataPrivate> d_ptr;
};
#endif
