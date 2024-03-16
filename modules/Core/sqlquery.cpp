#include "sqlquery.h"

#include "defs.h"
#include "datapipe.h"

SqlQuery::SqlQuery(SqlAdapter *adapter, const QSqlDatabase &db) : QObject {nullptr}
{
    m_sqlAdapter = adapter;
    m_sqlQuery = new QSqlQuery(db);
    m_connectionName = db.connectionName().isEmpty()?Defs::DEFAULT_CONNECTION:db.connectionName();
}

SqlQuery::SqlQuery(SqlAdapter *adapter, const QSqlError &error, const QSqlDatabase &db) : QObject {nullptr}
{
    m_sqlAdapter = adapter;
    m_sqlQuery = new QSqlQuery(db);
    m_connectionName = db.connectionName().isEmpty()?Defs::DEFAULT_CONNECTION:db.connectionName();

    m_isError = true;
    m_error = error;
}


SqlQuery::~SqlQuery()
{
    delete m_sqlQuery;
}

void SqlQuery::addBindValue(const QVariant &val, QSql::ParamType paramType)
{
    m_sqlQuery->addBindValue(val, paramType);
}

int SqlQuery::at() const
{
    return m_sqlQuery->at();
}

void SqlQuery::bindValue(const QString &placeholder, const QVariant &val, QSql::ParamType paramType)
{
    m_sqlQuery->bindValue(placeholder, val, paramType);
}

void SqlQuery::bindValue(int pos, const QVariant &val, QSql::ParamType paramType)
{
    m_sqlQuery->bindValue(pos, val, paramType);
}

QVariant SqlQuery::boundValue(const QString &placeholder) const
{
    return m_sqlQuery->boundValue(placeholder);
}

QVariant SqlQuery::boundValue(int pos) const
{
    return m_sqlQuery->boundValue(pos);
}

QVariantList SqlQuery::boundValues() const
{
    return m_sqlQuery->boundValues();
}

void SqlQuery::clear()
{
    return m_sqlQuery->clear();
}

const QSqlDriver *SqlQuery::driver() const
{
    return m_sqlQuery->driver();
}

bool SqlQuery::exec(const QString &query)
{
    // clear start error state

    return m_sqlQuery->exec(query);
}

bool SqlQuery::exec()
{
    // clear start error state
    m_isError = !m_sqlQuery->exec();
    return m_isError;
}

bool SqlQuery::execBatch(QSqlQuery::BatchExecutionMode mode)
{
    // clear start error state
    m_isError = false;
    return m_sqlQuery->execBatch(mode);
}

QString SqlQuery::executedQuery() const
{
    return m_sqlQuery->executedQuery();
}

void SqlQuery::finish()
{
    return m_sqlQuery->finish();
}

bool SqlQuery::first()
{
    return m_sqlQuery->first();
}

bool SqlQuery::isActive() const
{
    return m_sqlQuery->isActive();
}

bool SqlQuery::isForwardOnly() const
{
    return m_sqlQuery->isForwardOnly();
}

bool SqlQuery::isNull(int field) const
{
    return m_sqlQuery->isNull(field);
}

bool SqlQuery::isNull(const QString &name) const
{
    return m_sqlQuery->isNull(name);
}

bool SqlQuery::isSelect() const
{
    return m_sqlQuery->isSelect();
}

bool SqlQuery::isValid() const
{
    return m_sqlQuery->isValid();
}

bool SqlQuery::last()
{
    return m_sqlQuery->last();
}

QSqlError SqlQuery::lastError() const
{
    if(m_isError)
        return m_error;
    else
        return m_sqlQuery->lastError();
}

QVariant SqlQuery::lastInsertId() const
{
    return m_sqlQuery->lastInsertId();
}

QString SqlQuery::lastQuery() const
{
    return m_sqlQuery->lastQuery();
}

bool SqlQuery::next()
{
    return m_sqlQuery->next();
}

bool SqlQuery::nextResult()
{
    return m_sqlQuery->nextResult();
}

int SqlQuery::numRowsAffected() const
{
    return m_sqlQuery->numRowsAffected();
}

QSql::NumericalPrecisionPolicy SqlQuery::numericalPrecisionPolicy() const
{
    return m_sqlQuery->numericalPrecisionPolicy();
}

bool SqlQuery::prepare(const QString &query)
{
    return m_sqlQuery->prepare(query);
}

bool SqlQuery::previous()
{
    return m_sqlQuery->previous();
}

QSqlRecord SqlQuery::record() const
{
    return m_sqlQuery->record();
}

const QSqlResult *SqlQuery::result() const
{
    return m_sqlQuery->result();
}

bool SqlQuery::seek(int index, bool relative)
{
    return m_sqlQuery->seek(index, relative);
}

void SqlQuery::setForwardOnly(bool forward)
{
    m_sqlQuery->setForwardOnly(forward);
}

void SqlQuery::setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy)
{
    m_sqlQuery->setNumericalPrecisionPolicy(precisionPolicy);
}

int SqlQuery::size() const
{
    return m_sqlQuery->size();
}

void SqlQuery::swap(QSqlQuery &other)
{
    m_sqlQuery->swap(other);
}


QVariant SqlQuery::value(int index) const
{
    if(m_sqlQuery->isActive() && !m_sqlQuery->isValid())
        m_sqlQuery->next();

    return m_sqlQuery->value(index);
}


QVariant SqlQuery::value(const QString &name) const
{
    if(m_sqlQuery->isActive() && !m_sqlQuery->isValid())
        m_sqlQuery->next();

    return m_sqlQuery->value(name);
}

bool SqlQuery::isError()
{
    return m_isError;
}

bool SqlQuery::execute(const QJSValue &jsquery)
{
    QString queryString = jsquery.toString();

    bool res = exec(queryString);

    if(res){
        if(Datapipe::instance()->variable(Defs::DEBUG_LEVEL, 0).toInt()>=2)
            qDebug()<<queryString;
    }else{
        if(Datapipe::instance()->variable(Defs::DEBUG_LEVEL, 0).toInt()>=1){
            qCritical()<<queryString;
            qCritical()<<lastError().text();
        }
    }

    return res;
}

bool SqlQuery::insert(const QJSValue &jsquery)
{
    QString queryString = m_sqlAdapter->insert(jsquery);

    bool res = exec(queryString);

    if(res){
        if(Datapipe::instance()->variable(Defs::DEBUG_LEVEL, 0).toInt()>=2)
            qDebug()<<queryString;
    }else{
        if(Datapipe::instance()->variable(Defs::DEBUG_LEVEL, 0).toInt()>=1){
            qCritical()<<queryString;
            qCritical()<<lastError().text();
        }
    }

    return res;
}

bool SqlQuery::replace(const QJSValue &jsquery)
{
    QString queryString = m_sqlAdapter->replace(jsquery);

    bool res = exec(queryString);

    if(res){
        if(Datapipe::instance()->variable(Defs::DEBUG_LEVEL, 0).toInt()>=2)
            qDebug()<<queryString;
    }else{
        if(Datapipe::instance()->variable(Defs::DEBUG_LEVEL, 0).toInt()>=1){
            qCritical()<<queryString;
            qCritical()<<lastError().text();
        }
    }

    return res;
}

bool SqlQuery::remove(const QJSValue &jsquery)
{
    QString queryString = m_sqlAdapter->remove(jsquery);

    bool res = exec(queryString);

    if(res){
        if(Datapipe::instance()->variable(Defs::DEBUG_LEVEL, 0).toInt()>=2)
            qDebug()<<queryString;
    }else{
        if(Datapipe::instance()->variable(Defs::DEBUG_LEVEL, 0).toInt()>=1){
            qCritical()<<queryString;
            qCritical()<<lastError().text();
        }
    }

    return res;
}

bool SqlQuery::update(const QJSValue &jsquery)
{
    QString queryString = m_sqlAdapter->update(jsquery);

    bool res = exec(queryString);

    if(res){
        if(Datapipe::instance()->variable(Defs::DEBUG_LEVEL, 0).toInt()>=2)
            qDebug()<<queryString;
    }else{
        if(Datapipe::instance()->variable(Defs::DEBUG_LEVEL, 0).toInt()>=1){
            qCritical()<<queryString;
            qCritical()<<lastError().text();
        }
    }

    return res;
}

bool SqlQuery::select(const QJSValue &jsquery)
{
    QString queryString = m_sqlAdapter->select(jsquery);
    //qDebug()<<"before";
    bool res = exec(queryString);

    if(res){
        if(Datapipe::instance()->variable(Defs::DEBUG_LEVEL, 0).toInt()>=2)
            qDebug()<<queryString;
    }else{
        if(Datapipe::instance()->variable(Defs::DEBUG_LEVEL, 0).toInt()>=1){
            qCritical()<<queryString;
            qCritical()<<lastError().text();
        }
    }

    return res;
}
