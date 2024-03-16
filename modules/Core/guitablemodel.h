#ifndef QMLSQLTABLEMODEL_H
#define QMLSQLTABLEMODEL_H

#include <QSortFilterProxyModel>

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include <QVariant>
#include <QStringList>
#include <QPair>

#include "macros.h"
#include "qqmlintegration.h"


namespace ColumnType
{
Q_NAMESPACE
enum Value {
    UnknownColumn=0,

    PKColumn,
    FKColumn,
    StringColumn,
    IntColumn,
    BoolColumn,
    NumericColumn,
    DateColumn,
    DateTimeColumn,
    TimeColumn,
    LookupColumn,

    /*service columns*/
    DataColumn,
    ServiceColumn,
    ActionColumn,
    AppendRow,
    TotalsRow
};
Q_ENUM_NS(Value)
}

Q_DECLARE_METATYPE(ColumnType::Value)

namespace ModelRoles
{
Q_NAMESPACE
enum Value {
    Data=Qt::UserRole+1,
    Type,
    Digits,
    FieldName,
    SourceRow

};
Q_ENUM_NS(Value)
}

Q_DECLARE_METATYPE(ModelRoles::Value)

#define FilterPair QPair<int, QString>


class GuiTableModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_EXTENDED_NAMESPACE(ColumnType)

    UA_PROP_RW(QVariantList, fields, setFields)

    UA_PROP_RW(bool, serviceColumn, setServiceColumn)
    UA_PROP_RW(bool, actionColumn,  setActionColumn)
    UA_PROP_RW(bool, totalsRow,     setTotalsRow)
    UA_PROP_RW(bool, appendRow,     setAppendRow)

    UA_PROP_RO(int, lastRow)
    UA_PROP_RO(int, lastColumn)

    UA_PROP_RW(FilterPair , filter, setFilter)

public:
    UA_DEF(QString, TYPE, u"type"_qs)
    UA_DEF(QString, DIGITS, u"digits"_qs)
    UA_DEF(QString, FIELD, u"field"_qs)
    UA_DEF(QString, NAME, u"name"_qs)

public:
    explicit GuiTableModel(QObject *parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex& parent=QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex mapFromSource(const QModelIndex &source) const override;
    QModelIndex mapToSource(const QModelIndex &proxy) const override;
    QHash<int, QByteArray>roleNames() const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    void recalcLastColumn();
    void recalcLastRow();

    QVariant data(const QModelIndex &index, int role) const override;
    Q_INVOKABLE QString columnHeaderName(int column) const;

    void setSourceModel(QAbstractItemModel *sourceModel) override;

protected:
    bool filterAcceptsRow(int sourceRow,const QModelIndex &sourceParent) const override;
    bool filterAcceptsColumn(int source_column,const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;


public:
    Q_INVOKABLE int sourceRow(int row);

    void setRedirections(QMap<int, int> redirections);
    //void init();

private:
    QMap<int, int> redirectToSource;
    QMap<int, int> redirectFromSource;
};
#endif // QMLSQLTABLEMODEL_H
