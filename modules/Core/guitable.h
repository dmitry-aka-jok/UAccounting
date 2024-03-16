#ifndef GUITABLE_H
#define GUITABLE_H

#include <QItemSelectionModel>

#include "macros.h"

#include "guiitem.h"

#include "guitablemodel.h"
#include "asqltablemodel.h"


namespace GuiTableUsage
{
Q_NAMESPACE
enum Usage {
    Table = 1,
    SubTable,
    Selector
};
Q_ENUM_NS(Usage)

enum EditorType {
    NoEditor=0,
    Dialog,
    InRow,
};
Q_ENUM_NS(EditorType)
}


class GuiTable : public GuiItem
{
    Q_OBJECT
    QML_ELEMENT
    QML_EXTENDED_NAMESPACE(GuiTableUsage)

    // invitations
    UA_PROP_RW(QString, invitationAdd,    setInvitationAdd)
    UA_PROP_RW(QString, invitationEdit,   setInvitationEdit)
    UA_PROP_RW(QString, invitationDelete, setInvitationDelete)

    UA_PROP_RW(QString, execStack, setExecStack)

    // service columns & rows - proxy
    UA_PROP_RW(QString,      table,      setTable)
    UA_PROP_RW(QVariantList, fields,     setFields)
    UA_PROP_RW(QString,      mainTable,  setMainTable)
    UA_PROP_RW(QVariant,     conditions, setConditions)
    UA_PROP_RW(QString,      database,   setDatabase)

    UA_PROP_RW(bool, storeSettings, setStoreSettings)

    UA_PROXY_RW(bool, m_guiTableModel, serviceColumn, setServiceColumn)
    UA_PROXY_RW(bool, m_guiTableModel, actionColumn,  setActionColumn)
    UA_PROXY_RW(bool, m_guiTableModel, totalsRow,     setTotalsRow)
    UA_PROXY_RW(bool, m_guiTableModel, appendRow,     setAppendRow)

    Q_PROPERTY(GuiTableModel* guiTableModel READ guiTableModel CONSTANT)
    Q_PROPERTY(ASqlTableModel* sqlTableModel READ sqlTableModel CONSTANT)

    // filter and sort properties
    UA_PROP_RW(int, filterColumn, setFilterColumn)
    UA_PROP_RW(QString, filterString, setFilterString)
    UA_PROP_RW(int, sortField, setSortField)
    UA_PROP_RW(int, sortOrder, setSortOrder)

    UA_PROP_RW(GuiTableUsage::Usage, usage, setUsage)
    UA_PROP_RW(GuiTableUsage::EditorType, editorType, setEditorType)

public:
    GuiTable(QQuickItem *parent = 0);
    ~GuiTable();

public Q_SLOTS:
    void commit();

public:
    Q_INVOKABLE void init();
    Q_INVOKABLE GuiTableModel *guiTableModel();
    Q_INVOKABLE ASqlTableModel *sqlTableModel();

    void applySort();
    void applyFilter();

    void loadSettings();
    void saveSettings();

    Q_INVOKABLE int columnWidth(int column, const QString &type);
    Q_INVOKABLE void storeColumnWidth(int column, int width);
private:
    GuiTableModel m_guiTableModel;
    ASqlTableModel m_sqlTableModel;

    QMap<int, int> m_columnWidths;
    QMap<int, int>redirectFromSource;
    QMap<int, int>redirectToSource;

    bool saveColumnWidth;
    bool saveFilterColumn;
    bool saveSortField;
    bool saveSortOrder;
};

#endif // UATABLE_H
