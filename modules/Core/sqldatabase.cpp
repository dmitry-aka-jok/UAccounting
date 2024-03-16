#include "sqldatabase.h"

#include <QJSValueIterator>

#include "sqladaptersqlite.h"
#include "sqladapterpostgres.h"

SqlDatabase::SqlDatabase(const QString &connectionName, QObject *parent)
    : QObject{parent}, inited(false)
{
    //    connect(this, &Sql::dbStructureChanged, this, &Sql::recalcKeys);
    setConnectionName(connectionName);
}

void SqlDatabase::setConnectionName(const QString &connectionName)
{
    if(inited)
        return;
    if (m_connectionName == connectionName)
        return;
    m_connectionName = connectionName;
    emit connectionNameChanged();
}

void SqlDatabase::setDriver(const QString &driver)
{
    if(inited)
        return;
    if (m_driver == driver)
        return;
    m_driver = driver;
    emit driverChanged();
}

void SqlDatabase::setHost(const QString &host)
{
    if(inited)
        return;
    if (m_host == host)
        return;
    m_host = host;
    emit hostChanged();
}

void SqlDatabase::setDatabase(const QString &database)
{
    if(inited)
        return;
    if (m_database == database)
        return;
    m_database = database;
    emit databaseChanged();
}

void SqlDatabase::setUser(const QString &user)
{
    if(inited)
        return;
    if (m_user == user)
        return;
    m_user = user;
    emit userChanged();
}

void SqlDatabase::setPassword(const QString &password)
{
    if(inited)
        return;
    if (m_password == password)
        return;
    m_password = password;
    emit passwordChanged();
}

void SqlDatabase::setPort(const int &port)
{
    if(inited)
        return;
    if (m_port == port)
        return;
    m_port = port;
    emit portChanged();
}

void SqlDatabase::setOptions(const QString &options)
{
    if(inited)
        return;
    if (m_options == options)
        return;
    m_options = options;
    emit optionsChanged();
}

void SqlDatabase::setStructure(const QJSValue &structure)
{
    if (m_structure.equals(structure))
        return;
    m_structure = structure;

    if(inited)
        m_sqlAdapter->setStructure(structure);

    emit structureChanged();
}

QSqlDatabase SqlDatabase::currentDatabase()
{
    QSqlDatabase db;

    if(m_connectionName.isEmpty() || m_connectionName == Defs::DEFAULT_CONNECTION){
        if(!QSqlDatabase::contains())
            db = QSqlDatabase::addDatabase(m_driver);
        else
            db = QSqlDatabase::database();
    }else{
        if(!QSqlDatabase::contains(m_connectionName))
            db = QSqlDatabase::addDatabase(m_driver, m_connectionName);
        else
            db = QSqlDatabase::database(m_connectionName);
    }

    return db;
}


bool SqlDatabase::init()
{
    QMutexLocker locker (&m_initMutex);

    if(inited)
        return true;

    QSqlDatabase db = currentDatabase();

    if(!db.isOpen()){

        if(!m_user.isEmpty())
            db.setUserName(m_user);
        if(!m_password.isEmpty())
            db.setPassword(m_password);
        if(!m_host.isEmpty())
            db.setHostName(m_host);
        if(!m_database.isEmpty())
            db.setDatabaseName(m_database);
        if(m_port>0)
            db.setPort(m_port);

        if(!db.open()){
            m_lastError = db.lastError();

            return false;
        }
    }

    bool isFactory = false;
    if(m_driver=="QSQLITE"){
        m_sqlAdapter = new SqlAdapterSqlite();
        isFactory = true;
    }
    if(m_driver=="QPSQL"){
        m_sqlAdapter = new SqlAdapterPostgres();
        isFactory = true;
    }
    if(!isFactory)
        m_sqlAdapter = new SqlAdapter();

    m_sqlAdapter->setStructure(m_structure);

    inited = true;

 //   qDebug()<<"inited"<<m_database<<m_driver<<m_connectionName;
    m_sqlAdapter->validate();

    return true;
}

SqlQuery *SqlDatabase::query()
{
    if(!init())
        return new SqlQuery(m_sqlAdapter, m_lastError, currentDatabase());

    SqlQuery* sqlquery = new SqlQuery(m_sqlAdapter, currentDatabase());

    return sqlquery;
}

/*
SqlQuery *SqlDatabase::execute(const QJSValue &query)
{
    if(!init())
        return new SqlQuery(m_lastError, currentDatabase());

    SqlQuery* sqlquery = new SqlQuery(currentDatabase());

    QString queryString = "";
    if(query.isString())
        queryString = query.toString();

    if(!queryString.isEmpty()){
        sqlquery->exec(queryString);
    }else{
        QSqlError err("Empty query", "Empty query", QSqlError::StatementError);
        m_lastError.swap(err);
    }

    return sqlquery;
}


SqlQuery *SqlDatabase::insert(const QJSValue &query)
{
    if(!init())
        return new SqlQuery(m_lastError, currentDatabase());

    SqlQuery* sqlquery = new SqlQuery(currentDatabase());
    QJSEngine::setObjectOwnership(sqlquery, QJSEngine::JavaScriptOwnership);

    QString queryString = m_sqlAdapter->insert(query);

    if(!queryString.isEmpty())
        sqlquery->exec(queryString);

    m_lastQuery = queryString;
    return sqlquery;
}


SqlQuery *SqlDatabase::remove(const QJSValue &query)
{
    if(!init())
        return new SqlQuery(m_lastError, currentDatabase());

    SqlQuery* sqlquery = new SqlQuery(currentDatabase());
    QJSEngine::setObjectOwnership(sqlquery, QJSEngine::JavaScriptOwnership);

    QString queryString = m_sqlAdapter->remove(query);

    if(!queryString.isEmpty())
        sqlquery->exec(queryString);

    m_lastQuery = queryString;
    return sqlquery;
}



SqlQuery *SqlDatabase::update(const QJSValue &query)
{
    if(!init())
        return new SqlQuery(m_lastError, currentDatabase());

    SqlQuery* sqlquery = new SqlQuery(currentDatabase());
    QJSEngine::setObjectOwnership(sqlquery, QJSEngine::JavaScriptOwnership);

    QString queryString = m_sqlAdapter->update(query);

    m_lastQuery = queryString;
    if(!queryString.isEmpty())
        sqlquery->exec(queryString);

     return sqlquery;
}

SqlQuery *SqlDatabase::select(const QJSValue &query)
{
    if(!init())
        return new SqlQuery(m_lastError, currentDatabase());

    SqlQuery* sqlquery = new SqlQuery(currentDatabase());
    QJSEngine::setObjectOwnership(sqlquery, QJSEngine::JavaScriptOwnership);

    QString queryString = m_sqlAdapter->select(query);

    if(!queryString.isEmpty())
        sqlquery->exec(queryString);

    m_lastQuery = queryString;
    return sqlquery;
}

QString SqlDatabase::lastQuery()
{
    return m_lastQuery;
}

QSqlError SqlDatabase::lastError()
{
    return m_lastError;
}
*/

QString SqlDatabase::formatValue(const QVariant &val)
{
    if(!init())
        return QString();

    return m_sqlAdapter->formatValue(val);
}

QString SqlDatabase::formatValue(const QJSValue &val, const QString &symbol)
{
    if(!init())
        return QString();

    return m_sqlAdapter->formatValue(val, symbol);
}

QString SqlDatabase::formatTable(const QJSValue &val)
{
    if(!init())
        return QString();

    return m_sqlAdapter->formatTable(val);
}

QString SqlDatabase::formatField(const QJSValue &val)
{
    if(!init())
        return QString();

    return m_sqlAdapter->formatField(val);
}

QString SqlDatabase::formatAlias(const QString &val)
{
    if(!init())
        return QString();

    return m_sqlAdapter->formatAlias(val);
}

SqlFK SqlDatabase::foreignKey(const QString &table, const QString &foreignTable)
{
    if(!init())
        return SqlFK{};

    return m_sqlAdapter->foreignKey(table, foreignTable);
}

QString SqlDatabase::primaryKey(const QString &table)
{
    if(!init())
        return QString();

    return m_sqlAdapter->primaryKey(table);
}


