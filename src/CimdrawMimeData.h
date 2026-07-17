#ifndef CIMDRAWMIMEDATA_H_
#define CIMDRAWMIMEDATA_H_

#include <QMimeData>
#include <QGraphicsItem>
#include <QList>
#include <QScopedPointer>
#include <QPointF>

class CimdrawMimeDataPrivate;

class CimdrawMimeData : public  QMimeData
{
	Q_OBJECT
public:
	CimdrawMimeData(QList<QGraphicsItem* > items);
	~CimdrawMimeData();
	QList<QGraphicsItem*> items() const;
	QList<QPointF> delta() const;

    /** 生成一次粘贴用的新图元（不修改剪贴板模板），平移 delta，并重绑连线端点 */
    QList<QGraphicsItem*> instantiatePasteBatch(const QPointF& deltaScene) const;
private:
    Q_DECLARE_PRIVATE(CimdrawMimeData);
    QScopedPointer<CimdrawMimeDataPrivate> d_ptr;
};
#endif
