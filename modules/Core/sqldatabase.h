#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QObject>
#include <QQmlEngine>
#include <QMutex>

#include "defs.h"

#include "sqlquery.h"
#include "sqladapter.h"

class FType : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Enum share only")

public:
    enum DataTypes {
        Integer,
        Numeric,
        Float,
        Varchar,
        Boolean
    };
    Q_ENUM(DataTypes)
};


class SqlDatabase : public QObject
{
    Q_OBJECT
    QML_ELEMENT


     UA_PROP_RWS(QString, connectionName, setConnectionName)

     UA_PROP_RWS(QString, driver, setDriver)
     UA_PROP_RWS(QString, host, setHost)
     UA_PROP_RWS(QString, database, setDatabase)
     UA_PROP_RWS(QString, user, setUser)
     UA_PROP_RWS(QString, password, setPassword)
     UA_PROP_RWS(QString, options, setOptions)
     UA_PROP_RWS(int, port, setPort)

     UA_PROP_RWS(QJSValue, structure, setStructure)

public:
    explicit SqlDatabase(const QString& connectionName = Defs::DEFAULT_CONNECTION, QObject *parent = nullptr);

public Q_SLOTS:
    void setConnectionName(const QString& connectionName);
    void setDriver(const QString& driver);
    void setHost(const QString& host);
    void setDatabase(const QString& database);
    void setUser(const QString& user);
    void setPassword(const QString& password);
    void setPort(const int& port);
    void setOptions(const QString& options);
    void setStructure(const QJSValue& structure);


public:
    Q_INVOKABLE bool init();

    Q_INVOKABLE SqlQuery* query();

    QString formatValue(const QVariant &val);
    QString formatValue(const QJSValue &val, const QString &symbol = Defs::EMPTY_STRING);
    QString formatTable(const QJSValue &val);
    QString formatField(const QJSValue &val);
    QString formatAlias(const QString &val);

    SqlFK foreignKey(const QString &table, const QString &foreignTable);
    QString primaryKey(const QString &table);

    QSqlDatabase currentDatabase();
private:

    QSqlError m_lastError;
   // QString m_lastQuery;
    bool inited;
    SqlAdapter* m_sqlAdapter;

    QMutex m_initMutex;
};


#endif // SQLDATABASE_H
