#include "tool/CimdrawTool.h"
#include "CimdrawListWidgetItem.h"

class CimdrawListWidgetItemPrivate
{
    Q_DECLARE_PUBLIC(CimdrawListWidgetItem)
public:
    CimdrawListWidgetItemPrivate(CimdrawListWidgetItem* item, CimdrawTool* tool)
        :q_ptr(item),tool(tool)
    {
    }
    //对应tool
    CimdrawTool* tool;
    //显示文字
    QVariant data;
    CimdrawListWidgetItem* q_ptr;
};

CimdrawListWidgetItem::CimdrawListWidgetItem(const QSize&size, CimdrawTool* tool, const QVariant& data, QListWidget *view)
    :d_ptr(new CimdrawListWidgetItemPrivate(this,tool))
{
    Q_D(CimdrawListWidgetItem);
    if(tool)
    {
        setIcon(d_ptr->tool->getIcon(size,data));

        setText(d_ptr->tool->getName());
    }
}

CimdrawTool* CimdrawListWidgetItem::getTool()
{
    return d_ptr->tool;
}

const QVariant& CimdrawListWidgetItem::getData()
{
    return d_ptr->data;
}

QString CimdrawListWidgetItem::getText() const
{
    if(d_ptr->tool)
    {
        return d_ptr->tool->getName();
    }
    return QString();
}

CREATE_MODE CimdrawListWidgetItem::getCreateMode()
{
    if(d_ptr->tool)
    {
        return d_ptr->tool->getCreateMode();
    }
    return CREATE_MODE::CREATE_MODE_NONE;
}
