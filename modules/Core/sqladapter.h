#ifndef SQLADAPTER_H
#define SQLADAPTER_H

#include <QObject>
#include <QJSValue>

#include "defs.h"

struct SqlFK{
    QString field;
    QString foreign_table;
    QString foreign_key;
};

struct SqlPK{
    QString field;
};


class SqlAdapter : public QObject
{
public:
    explicit SqlAdapter(QObject *parent = nullptr);

    void setStructure(const QJSValue &structure);

    virtual bool validate();

    virtual QString insert(const QJSValue &query);
    virtual QString replace(const QJSValue &query);
    virtual QString remove(const QJSValue &query);
    virtual QString update(const QJSValue &query);
    virtual QString select(const QJSValue &query);

protected:
    virtual QPair<QString, QString> table(const QJSValue &query);
    virtual QString join(const QJSValue &query, const QString &table, const QString &alias);
    virtual QString prefix(const QJSValue &query);
    virtual QString fields(const QJSValue &query);
    virtual QString limit(const QJSValue &query);
    virtual QString where(const QJSValue &query);
    virtual QString order(const QJSValue &query);
    virtual QString group(const QJSValue &query);
    virtual QString having(const QJSValue &query);

    virtual QString tag(const QJSValue &query, const QString &tag);

    virtual QString set(const QJSValue &query);
    virtual QPair<QString, QString> values(const QJSValue &query);
    virtual QString returning(const QJSValue &query);


public:
    virtual QString formatValue(const QVariant &val);
    virtual QString formatValue(const QJSValue &val, const QString &symbol = Defs::EMPTY_STRING);
    virtual QString formatTable(const QJSValue &val);
    virtual QString formatField(const QJSValue &val);
    virtual QString formatAlias(const QString &val);

    virtual SqlFK foreignKey(const QString &table, const QString &joinTable);
    virtual QString primaryKey(const QString &table);

private:
    QMultiMap<QString, SqlFK> m_fk;
    QMap<QString, QString> m_pk;
};

#endif // SQLADAPTER_H
