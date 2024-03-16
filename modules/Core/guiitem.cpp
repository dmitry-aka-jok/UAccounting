#include "guiitem.h"

#include "datapipe.h"

GuiItem::GuiItem(QQuickItem *parent) : QQuickItem(parent)
{
    m_iid = Datapipe::instance()->nextIid();
}

/*
void GuiItem::findFocus()
{
    if (! findFocus_(this) )
        qDebug()<<"No active focus";
}

bool GuiItem::findFocus_(QQuickItem *parent)
{
    if (parent==nullptr)
        return false;

    const QList<QQuickItem *> &children = parent->childItems();
    QQuickItem *obj;
    for (int i = 0; i < children.size(); ++i) {
        obj = children.at(i);
        QVariant fieldProperty = obj->property("activeFocus");
        if(fieldProperty.isValid() && fieldProperty.toBool()){
//            qDebug()<<"Active focus object: "<<obj;
            return true;
        }
        if (findFocus_(obj))
            return true;
    }
    return false;
}
*/
