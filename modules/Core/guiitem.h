#ifndef GUIITEM_H
#define GUIITEM_H

#include <QObject>
#include <QQuickItem>

#include "macros.h"

class GuiItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    UA_PROP_RW(int, iid, setIid)
    UA_PROP_RW(QString, taskName, setTaskName)

public:
    GuiItem(QQuickItem *parent = 0);

//     Q_INVOKABLE void findFocus();
// private:
//     bool findFocus_(QQuickItem *parent);
};

#endif // GUIITEM_H
