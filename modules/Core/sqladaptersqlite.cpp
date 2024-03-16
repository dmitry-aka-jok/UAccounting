#include "sqladaptersqlite.h"

SqlAdapterSqlite::SqlAdapterSqlite()
{
}

bool SqlAdapterSqlite::validate()
{
    return false;
}

QString SqlAdapterSqlite::replace(const QJSValue &query)
{
    QString queryString;

    if(query.isString()){
        queryString = query.toString();
        if (!queryString.startsWith("REPLACE", Qt::CaseInsensitive))
            queryString = u"REPLACE %1"_qs.arg(queryString);
    }

    if (query.isObject()) {
        QPair<QString, QString> ptable = table(query);
        QPair<QString, QString> pvalues = values(query);

        queryString = u"REPLACE INTO %1%2%3 VALUES (%4)%5"_qs.arg(
            ptable.first,
            ptable.second.isEmpty()?Defs::EMPTY_STRING:u" %1"_qs.arg(ptable.second),

            pvalues.first.isEmpty()?Defs::EMPTY_STRING:u" (%1)"_qs.arg(pvalues.first),
            pvalues.second,

            returning(query)
            );
    }

    return queryString;

}
