#ifndef ASQLTABLEMODEL_H
#define ASQLTABLEMODEL_H

#include "qqmlintegration.h"
#include <QAbstractTableModel>
#include <QObject>
#include <QSqlRecord>
#include <QJSValue>

#include "guitablemodel.h"
#include "macros.h"


class ASqlTableModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT

    UA_PROP_RW(QString, connectionName, setConnectionName)

    UA_PROP_RW(QString, table, setTable)
    UA_PROP_RW(QVariantList, fields, setFields)
    UA_PROP_RW(QString, filter, setFilter)

public:
    UA_DEF(QString, TABLE, u"table"_qs)
    UA_DEF(QString, TYPE, u"type"_qs)
    UA_DEF(QString, NAME, u"name"_qs)
    UA_DEF(QString, VISIBLE, u"visible"_qs)
    UA_DEF(QString, DIGITS, u"digits"_qs)
    UA_DEF(QString, FIELD, u"field"_qs)
    UA_DEF(QString, CALCULATED, u"calculated"_qs)
    UA_DEF(QString, FOREIGN_TABLE, u"foreign_table"_qs)
    UA_DEF(QString, FOREIGN_KEY, u"foreign_key"_qs)
    UA_DEF(QString, FOREIGN_NAME, u"foreign_name"_qs)

public:
    explicit ASqlTableModel(QObject *parent = nullptr);

    // manipulate transactions
//    bool transaction();
    virtual bool submit();
    virtual void revert();

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int = Qt::EditRole);
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
//    virtual QModelIndexList match(const QModelIndex & start, int role, const QVariant & value,
//                                  int hits = 1,
//                                  Qt::MatchFlags flags = Qt::MatchFlags( Qt::MatchStartsWith | Qt::MatchWrap )) const;

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, const QString &columnName, const QModelIndex &parent = QModelIndex()) const;

    int shiftDeleted(int row) const;

//    Q_INVOKABLE bool appendRow();

    bool isDirty() const;
    bool isDirty(const QModelIndex &index) const;

    QSqlRecord record() const;
    QSqlRecord record(int row) const;

    QString lastQuery() const;
    QSqlError lastError() const;

    QVariantMap rowKey(int row);
    QVariantMap rowMap(int row);

public:
    Q_INVOKABLE bool select();

private:
    QSqlRecord m_record;
    QList<QSqlRecord> m_records;
    QMap<int, QSqlRecord> m_insertedRecords, m_updatedRecords;
    QMap<int, int> m_deletedRecords;

    QList<ColumnType::Value> m_fieldTypes;

    QString m_lastQuery;
    QSqlError m_lastError;
    //, originalRecords_;
};

#endif // ASQLTABLEMODEL_H
