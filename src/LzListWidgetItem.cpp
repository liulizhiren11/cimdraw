#include "tool/LzTool.h"
#include "LzListWidgetItem.h"

class LzListWidgetItemPrivate
{
    Q_DECLARE_PUBLIC(LzListWidgetItem)
public:
    LzListWidgetItemPrivate(LzListWidgetItem* item, LzTool* tool)
        :q_ptr(item),tool(tool)
    {
    }
    //对应tool
    LzTool* tool;
    //显示文字
    QVariant data;
    LzListWidgetItem* q_ptr;
};

LzListWidgetItem::LzListWidgetItem(const QSize&size, LzTool* tool, const QVariant& data, QListWidget *view)
    :d_ptr(new LzListWidgetItemPrivate(this,tool))
{
    Q_D(LzListWidgetItem);
    if(tool)
    {
        setIcon(d_ptr->tool->getIcon(size,data));

        setText(d_ptr->tool->getName());
    }
}

LzTool* LzListWidgetItem::getTool()
{
    return d_ptr->tool;
}

const QVariant& LzListWidgetItem::getData()
{
    return d_ptr->data;
}

QString LzListWidgetItem::getText() const
{
    if(d_ptr->tool)
    {
        return d_ptr->tool->getName();
    }
    return QString();
}

CREATE_MODE LzListWidgetItem::getCreateMode()
{
    if(d_ptr->tool)
    {
        return d_ptr->tool->getCreateMode();
    }
    return CREATE_MODE::CREATE_MODE_NONE;
}
