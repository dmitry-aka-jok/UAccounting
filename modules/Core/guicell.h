#ifndef GUICELL_H
#define GUICELL_H

#include <QQuickItem>

#include "macros.h"
#include "asqltablemodel.h"


class GuiCell : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    // UA_PROP_RW(QString, sqlField, setSqlField)
    // UA_PROP_RW(int, sqlRow, setSqlRow)
    // UA_PROP_RW(ASqlTableModel*, sqlModel, setSqlModel)
    // UA_PROP_RW(bool, autosave, setAutosave)

    UA_PROP_RW(QVariant, value, setValue)
    UA_PROP_RW(int, digits, setDigits)

public:
    GuiCell(QQuickItem *parent = 0);

};

#endif // GUICELL_H
