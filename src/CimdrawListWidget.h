#ifndef CIMDRAWLISTWIDGET_H
#define CIMDRAWLISTWIDGET_H

#include <QListWidget>

class CimdrawListWidget : public QListWidget
{
    Q_OBJECT
public:
    CimdrawListWidget(QWidget* parent = nullptr);
protected:
    virtual void mousePressEvent(QMouseEvent* event);
};

#endif
