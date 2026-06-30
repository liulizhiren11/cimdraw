#ifndef LZLISTWIDGET_H
#define LZLISTWIDGET_H

#include <QListWidget>

class LzListWidget : public QListWidget
{
    Q_OBJECT
public:
    LzListWidget(QWidget* parent = nullptr);
protected:
    virtual void mousePressEvent(QMouseEvent* event);
};

#endif
