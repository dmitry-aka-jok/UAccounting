#include "sqladapterpostgres.h"
#include <QJSValueIterator>

SqlAdapterPostgres::SqlAdapterPostgres() {}

bool SqlAdapterPostgres::validate()
{
    return false;
}


/*
INSERT INTO table_name (column1, column2, ...)
VALUES (value1, value2, ...)
ON CONFLICT (conflict_column)
DO NOTHING | DO UPDATE SET column1 = value1, column2 = value2, ...;
*/
QString SqlAdapterPostgres::replace(const QJSValue &query)
{
    QString queryString;

    if(query.isString()){
        queryString = query.toString();
        if (!queryString.startsWith("UPSERT", Qt::CaseInsensitive))
            queryString = u"UPSERT %1"_qs.arg(queryString);
    }

    if (query.isObject()) {
        QPair<QString, QString> ptable = table(query);
        QPair<QString, QString> pvalues = values(query);

        QJSValue tmpJS = query.property("values");
        QStringList setList;
        QJSValueIterator it(tmpJS);
        while (it.hasNext()) {
            it.next();
            setList<<(formatField(it.name())
                        +formatValue(it.value(), Defs::EQ));
        }
        QString setS = setList.join(Defs::COMMA+Defs::SPACE);

        QString keyS = tag(query, "key");

        queryString = u"UPSERT INTO %1%2%3 VALUES (%4) ON CONFLICT%5 %6"_qs.arg(
            ptable.first,
            ptable.second.isEmpty()?Defs::EMPTY_STRING:u" %1"_qs.arg(ptable.second),

            pvalues.first.isEmpty()?Defs::EMPTY_STRING:u" (%1)"_qs.arg(pvalues.first),
            pvalues.second,

            keyS.isEmpty()?u""_qs:u" (%1)"_qs.arg(keyS),

            setS.isEmpty()?u"DO NOTHING"_qs:u"DO UPDATE SET %1"_qs.arg(setS)
            );
    }

    return queryString;

}
