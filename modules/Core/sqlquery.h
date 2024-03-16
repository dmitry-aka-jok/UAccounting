#ifndef SQLQUERY_H
#define SQLQUERY_H

#include <QObject>
#include <QQmlEngine>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
#include <QSqlError>

#include "sqladapter.h"

// QML adapter to SQL query
class SqlQuery : public QObject
{
    Q_OBJECT
//    QML_ELEMENT

public:
    SqlQuery(SqlAdapter *adapter, const QSqlDatabase &db = QSqlDatabase());
    SqlQuery(SqlAdapter *adapter, const QSqlError &error, const QSqlDatabase &db = QSqlDatabase());

    ~SqlQuery();

    void addBindValue(const QVariant &val, QSql::ParamType paramType = QSql::In);
    int at() const;
    void bindValue(const QString &placeholder, const QVariant &val, QSql::ParamType paramType = QSql::In);
    void bindValue(int pos, const QVariant &val, QSql::ParamType paramType = QSql::In);
    QVariant boundValue(const QString &placeholder) const;
    QVariant boundValue(int pos) const;
    QVariantList boundValues() const;
    void clear();
    const QSqlDriver *driver() const;
    bool exec(const QString &query);
    bool exec();
    bool execBatch(QSqlQuery::BatchExecutionMode mode = QSqlQuery::ValuesAsRows);
    QString executedQuery() const;
    void finish();
    Q_INVOKABLE bool first();
    Q_INVOKABLE bool isActive() const;
    bool isForwardOnly() const;
    bool isNull(int field) const;
    bool isNull(const QString &name) const;
    bool isSelect() const;
    bool isValid() const;
    Q_INVOKABLE bool last();
    QSqlError lastError() const;
    QVariant lastInsertId() const;
    Q_INVOKABLE QString lastQuery() const;
    bool nextResult();
    int numRowsAffected() const;
    QSql::NumericalPrecisionPolicy numericalPrecisionPolicy() const;
    bool prepare(const QString &query);
    bool previous();
    QSqlRecord record() const;
    const QSqlResult *result() const;
    bool seek(int index, bool relative = false);
    void setForwardOnly(bool forward);
    void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy);
    int size() const;
    void swap(QSqlQuery &other);

    Q_INVOKABLE bool next();
    Q_INVOKABLE QVariant value(int index) const;
    Q_INVOKABLE QVariant value(const QString &name) const;

    Q_INVOKABLE bool isError();

    Q_INVOKABLE bool execute(const QJSValue &jsquery);
    Q_INVOKABLE bool insert(const QJSValue &jsquery);
    Q_INVOKABLE bool replace(const QJSValue &jsquery);
    Q_INVOKABLE bool remove(const QJSValue &jsquery);
    Q_INVOKABLE bool update(const QJSValue &jsquery);
    Q_INVOKABLE bool select(const QJSValue &jsquery);

private:
    QSqlQuery* m_sqlQuery;
    QString m_connectionName;
    SqlAdapter* m_sqlAdapter;

    bool m_isError;

    QSqlError m_error;
};

#endif // SQLQUERY_H
