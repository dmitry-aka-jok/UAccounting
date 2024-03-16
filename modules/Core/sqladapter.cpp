#include "sqladapter.h"

#include <QJSValueIterator>

#include <QRegularExpression>
#include <QRegularExpressionMatch>


SqlAdapter::SqlAdapter(QObject *parent): QObject{parent}
{
}

void SqlAdapter::setStructure(const QJSValue &structure)
{
    m_pk.clear();
    if(structure.hasProperty("primary_keys")){
        QJSValue tmpJS = structure.property("primary_keys");

        if(tmpJS.isArray()){
            const int length = tmpJS.property("length").toInt();
            for (int i = 0; i < length; ++i) {
                QJSValue prop = tmpJS.property(i);
//                qDebug()<<i<<"ins pk"<<prop.property("table").toString()<<prop.property("field").toString();
                m_pk.insert(prop.property("table").toString(), prop.property("field").toString());
            }
        }
    }

    m_fk.clear();
    if(structure.hasProperty("foreign_keys")){
        QJSValue tmpJS = structure.property("foreign_keys");

        if(tmpJS.isArray()){
            const int length = tmpJS.property("length").toInt();
            for (int i = 0; i < length; ++i) {
                QJSValue prop = tmpJS.property(i);
                SqlFK fk;
                fk.field = prop.property("field").toString();
                fk.foreign_table = prop.property("foreign_table").toString();
                fk.foreign_key = prop.property("foreign_key").toString();
                m_fk.insert(prop.property("table").toString(), fk);
            }
        }
    }

}


bool SqlAdapter::validate()
{
    return false;
}


QString SqlAdapter::formatValue(const QVariant &val)
{
    if (val.typeId() == QMetaType::QString)
        return u"'%1'"_qs.arg(val.toString());
    if (val.typeId() == QMetaType::QDate || val.typeId() == QMetaType::QTime || val.typeId() == QMetaType::QDateTime)
        return u"'%1'"_qs.arg(val.toString());

    return val.toString();
}

QString SqlAdapter::formatTable(const QJSValue &val)
{
    QString s = val.toString();
    if(s.contains(Defs::SPACE) || s.contains(Defs::DOT)){
        s = u"\"%1\""_qs.arg(s);
    }

    return s;
}

QString SqlAdapter::formatField(const QJSValue &val)
{
    QString s = val.toString();
    if(s.contains(Defs::SPACE)){
        s = u"\"%1\""_qs.arg(s);
    }

    return s;
}

QString SqlAdapter::formatAlias(const QString &val)
{
    QString curr = val;
    curr.replace('.', '_');
    return curr;
}

SqlFK SqlAdapter::foreignKey(const QString &table, const QString &joinTable)
{
    auto it = m_fk.constFind(table);
    while (it != m_fk.constEnd() && it.key()==table) {
        if(it.key()==joinTable)
            return it.value();

        ++it;
    }
    return SqlFK{};
}

QString SqlAdapter::primaryKey(const QString &table)
{
    return m_pk.value(table);
}

QString SqlAdapter::formatValue(const QJSValue &val, const QString &symbol)
{
    //qDebug()<<val.toString()<<val.isString()<<val.isNumber();


    if (val.isString())
        return u"%1'%2'"_qs.arg(symbol, val.toString());

    if (val.isDate())
        return u"%1'%2'"_qs.arg(symbol, val.toString());

    if (val.isNull()){
        if(symbol==Defs::EQ)
            return "is null";
        if(symbol==Defs::NE)
            return "is not null";
        return "null";
    }

    return symbol+val.toString();
}

/*
insert(arg)
arg = query string | query jsObject
query object = {
                 into|table: string
                 alias: string
                 values: jsObject {field:value...} | string
                 returning: string | jsArray[field...]
               }
*/
QString SqlAdapter::insert(const QJSValue &query)
{
    QString queryString;

    if(query.isString()){
        queryString = query.toString();
        if (!queryString.startsWith("INSERT", Qt::CaseInsensitive))
            queryString = u"INSERT %1"_qs.arg(queryString);
    }

    if (query.isObject()) {
        QPair<QString, QString> ptable = table(query);
        QPair<QString, QString> pvalues = values(query);

        queryString = u"INSERT INTO %1%2%3 VALUES (%4)%5"_qs.arg(
            ptable.first,
            ptable.second.isEmpty()?Defs::EMPTY_STRING:u" %1"_qs.arg(ptable.second),

            pvalues.first.isEmpty()?Defs::EMPTY_STRING:u" (%1)"_qs.arg(pvalues.first),
            pvalues.second,

            returning(query)
            );
    }

    return queryString;
}

/*
replace(arg)
arg = query string | query jsObject
query object = {
                 into|table: string
                 alias: string
                 values: jsObject {field:value...} | string
                 returning: string | jsArray[field...]
               }
*/
QString SqlAdapter::replace(const QJSValue &query)
{
    qDebug()<<"wtf";
    return QString();
}

/*
remove(arg)
arg = query string | query jsObject
query jsObject = {
                 from|table: (table name string | jsObject.name)
                 where: jsObject fields | fields: string
               }
jsObject fields= { field name string : QVariant field value }
jsArray fields = [...string field name | jsObject.name]
*/
QString SqlAdapter::remove(const QJSValue &query)
{
    QString queryString;
    if(query.isString()){
        queryString = query.toString();
        if (!queryString.startsWith("DELETE", Qt::CaseInsensitive))
            queryString = u"DELETE %1"_qs.arg(queryString);
    }

    if (query.isObject()) {
        QPair<QString, QString> ptable = table(query);

        queryString = u"DELETE FROM %1%2%3"_qs.arg(
            ptable.first,
            ptable.second.isEmpty()?Defs::EMPTY_STRING:u" %1"_qs.arg(ptable.second),
            where(query)
            );
    }

    return queryString;
}


/*
update(arg)
arg = query string | query jsObject
query jsObject = {
                 table: (table name string | jsObject.name)
                 set: jsObject fields | fields: string
                 where: jsObject fields | fields: string
               }
jsObject fields= { field name string : QVariant field value }
jsArray fields = [...string field name | jsObject.name]
*/
QString SqlAdapter::update(const QJSValue &query)
{
    QString queryString;
    if(query.isString()){
        queryString = query.toString();
        if (!queryString.startsWith("UPDATE", Qt::CaseInsensitive))
            queryString = u"UPDATE %1"_qs.arg(queryString);
    }

    if (query.isObject()) {
        QPair<QString, QString> ptable = table(query);

        queryString = u"UPDATE %1%2 SET %3%4"_qs.arg(
            ptable.first,
            ptable.second.isEmpty()?Defs::EMPTY_STRING:u" %1"_qs.arg(ptable.second),
            set(query),
            where(query)
            );
    }


    return queryString;
}

/*
select(arg)
arg = query_string | jsObject {query_object}
query_object = {
                table|from: table_string
                alias: string
                join: join_string | jsArray[join_string...]
                fields|values: jsArray [field_string...] | string
                where: jsObject {field:value...} | string
                group: jsArray [field_string...] | string
                order: jsArray [field_string...] | string
                having: jsArray [field_string...] | string
                limit: jsArray [field_string...] | string | number
               }
table_string = table | table alias
join_string = table | table alias | table alias on condition
field_string = field
*/

QString SqlAdapter::select(const QJSValue &query)
{
    QString queryString;
    if(query.isString()){
        queryString = query.toString();
        if (!queryString.startsWith("SELECT", Qt::CaseInsensitive))
            queryString = u"SELECT %1"_qs.arg(queryString);
    }
    if (query.isObject()) {
        QPair<QString, QString> ptable = table(query);

        queryString = u"SELECT %1%2 FROM %3%4%5%6%7%8%9%10"_qs.arg(
            prefix(query),
            fields(query),
            ptable.first,
            ptable.second.isEmpty()?Defs::EMPTY_STRING:u" %1"_qs.arg(ptable.second),
            join(query, ptable.first, ptable.second),
            where(query),
            limit(query),
            group(query),
            order(query),
            having(query));
    }

        return queryString;

}

QPair<QString, QString> SqlAdapter::table(const QJSValue &query)
{
    QString table, alias;

    if(query.hasProperty("alias"))
        alias = formatTable(query.property("alias"));

    if(query.hasProperty("from")) // for select
        table = query.property("from").toString();
    if(query.hasProperty("into")) // for insert
        table = query.property("into").toString();
    else if(query.hasProperty("table"))
        table = query.property("table").toString();

    if(table.contains(Defs::SPACE)){
        alias = table.mid(table.indexOf(Defs::SPACE)+1);
        table = table.left(table.indexOf(Defs::SPACE));
    }

    return QPair<QString, QString>(table, alias);
}

QString SqlAdapter::join(const QJSValue &query, const QString &table, const QString &alias)
{
    QString result;

    QMap<QString, QString> aliases;
    aliases.insert(table, alias);

    QStringList tables;
    tables.append(table);

    // join: string | jsArray [table | table alias | table alias on structure ]
    if (query.hasProperty("join")){
        QList<QJSValue> joinJSList;
        QJSValue joinJS = query.property("join");

        if(joinJS.isArray()){
            //QStringList fieldsAr;
            const int length = joinJS.property("length").toInt();
            for (int i = 0; i < length; ++i) {
                joinJSList<<joinJS.property(i);
            }
        } else
            joinJSList << joinJS;


        foreach(joinJS, joinJSList){
            QString svar = joinJS.toString().toLower();

            if ( svar.contains("join") )
            {
                static QRegularExpression re("join (\\w+) (\\w+)"); // TODO check alias
                QRegularExpressionMatch match = re.match(svar);
                if (match.hasMatch()) {
                    QString joinTable = match.captured(1);
                    QString joinAlias = match.captured(2);
                    aliases.insert(joinTable, formatAlias(joinAlias));
                }
            }
            else
            {
                QString joinTable = svar;
                QString joinAlias;

                if (joinTable.contains(Defs::SPACE)){
                    joinAlias = joinTable.mid(joinTable.indexOf(Defs::SPACE)+1);
                    joinTable = joinTable.left(joinTable.indexOf(Defs::SPACE));
                }else{
                    joinAlias = joinTable;
                }

                QString joinOn;

                aliases.insert(joinTable, formatAlias(joinAlias));

                // search in all already appended tables
                foreach (auto tableMain, tables){

                    auto fks = foreignKey(tableMain, joinTable);
                    if(!fks.field.isEmpty() && !fks.foreign_key.isEmpty()){
                        joinOn = u" ON %1.%2 = %3.%4"_qs.arg(
                            aliases.value(tableMain),
                            fks.field,
                            aliases.value(joinTable),
                            fks.foreign_key);
                        break;
                    }
                    if(!joinOn.isEmpty()) break;
                }

                // if join on not finded - try to check joined table - maybe it got reference fo main
                if(joinOn.isEmpty()){
                    // search in all already appended tables
                    foreach (auto tableMain, tables){

                        auto fks = foreignKey(joinTable, tableMain);
                        if(!fks.field.isEmpty() && !fks.foreign_key.isEmpty()){
                            joinOn = u" ON %1.%2 = %3.%4"_qs.arg(
                                aliases.value(tableMain),
                                fks.foreign_key,
                                aliases.value(joinTable),
                                fks.field);
                            break;
                        }
                        if(!joinOn.isEmpty()) break;
                    }
                }



                tables.append(svar);

                svar = u"JOIN %1%2%3"_qs.arg(
                    formatTable(joinTable),
                    joinAlias.isEmpty()?Defs::EMPTY_STRING:u" %1"_qs.arg(joinAlias),
                    joinOn);
            }


            result += Defs::SPACE + svar;
        }
    }

    return result;
}

QString SqlAdapter::prefix(const QJSValue &query)
{
    QString result;

    if(query.hasProperty("first")){
        QJSValue tmpJS = query.property("first");
        result += u"FIRST %1 "_qs.arg(tmpJS.toString());
    }

    if(query.hasProperty("distinct")){
        result += u"DISTINCT "_qs;
    }

    return result;

}

QString SqlAdapter::fields(const QJSValue &query)
{
    QString result;

    if(query.hasProperty("fields")){
        QJSValue tmpJS = query.property("fields");

        if(tmpJS.isArray()){
            QStringList tmpAr;
            const int length = tmpJS.property("length").toInt();
            for (int i = 0; i < length; ++i) {
                tmpAr<<formatField(tmpJS.property(i));
            }
            result = tmpAr.join(Defs::COMMA+Defs::SPACE);
        }else
            result = tmpJS.toString();
    }else{
        result = Defs::ASTERISK;
    }

    return result;
}


QString SqlAdapter::limit(const QJSValue &query)
{
    QString result;

    if(query.hasProperty("limit")){
        result = u" LIMIT %1"_qs.arg(query.property("limit").toString());
    }

    return result;
}

QString SqlAdapter::where(const QJSValue &query)
{
    QString result;

    if(query.hasProperty("where")){
        QString tmpres;

        QJSValue tmpJS = query.property("where");
        if(tmpJS.isObject()){
            QStringList tmpList;

            QJSValueIterator it(tmpJS);
            while (it.hasNext()) {
                it.next();

                tmpList<<(formatTable(it.name())
                            +formatValue(it.value(), Defs::EQ));
            }
            tmpres = u"(%1)"_qs.arg(tmpList.join(") AND ("));

        }else{
            tmpres = tmpJS.toString();
        }
        result = u" WHERE %1"_qs.arg(tmpres);
    }

    return result;
}

QString SqlAdapter::order(const QJSValue &query)
{
    QString result;

    if(query.hasProperty("order")){
        QString res;
        QJSValue tmpJS = query.property("order");

        if(tmpJS.isArray()){
            QStringList tmpAr;
            const int length = tmpJS.property("length").toInt();
            for (int i = 0; i < length; ++i) {
                tmpAr<<tmpJS.property(i).toString();
            }
            res = tmpAr.join(Defs::COMMA+Defs::SPACE);
        }else
            res = tmpJS.toString();

        result = u" ORDER BY %1"_qs.arg(res);
    }

    return result;
}

QString SqlAdapter::group(const QJSValue &query)
{
    QString result;

    if(query.hasProperty("group")){
        QString res;
        QJSValue tmpJS = query.property("group");

        if(tmpJS.isArray()){
            QStringList tmpAr;
            const int length = tmpJS.property("length").toInt();
            for (int i = 0; i < length; ++i) {
                tmpAr<<tmpJS.property(i).toString();
            }
            res = tmpAr.join(Defs::COMMA);
        }else
            res = tmpJS.toString();

        result = u" GROUP BY %1"_qs.arg(res);
    }

    return result;
}

QString SqlAdapter::having(const QJSValue &query)
{
    QString result;

    if(query.hasProperty("having")){
        QString res;
        QJSValue tmpJS = query.property("having");

        if(tmpJS.isArray()){
            QStringList tmpAr;
            const int length = tmpJS.property("length").toInt();
            for (int i = 0; i < length; ++i) {
                tmpAr<<tmpJS.property(i).toString();
            }
            res = tmpAr.join(Defs::COMMA);
        }else
            res = tmpJS.toString();

        result = u" HAVING %1"_qs.arg(res);
    }

    return result;
}

QString SqlAdapter::tag(const QJSValue &query, const QString &tag)
{
    QString result;

    if(query.hasProperty(tag)){
        QString res;
        QJSValue tmpJS = query.property(tag);

        if(tmpJS.isArray()){
            QStringList tmpAr;
            const int length = tmpJS.property("length").toInt();
            for (int i = 0; i < length; ++i) {
                tmpAr<<tmpJS.property(i).toString();
            }
            res = tmpAr.join(Defs::COMMA);
        }else
            res = tmpJS.toString();

        result = res;
    }

    return result;
}

QString SqlAdapter::set(const QJSValue &query)
{
    QString result;

    if(query.hasProperty("set")){
        QJSValue tmpJS = query.property("set");

        if(tmpJS.isObject()){
            QStringList tmpList;

            QJSValueIterator it(tmpJS);
            while (it.hasNext()) {
                it.next();
                tmpList<<(formatField(it.name())
                            +formatValue(it.value(), Defs::EQ));
            }
            result = tmpList.join(Defs::COMMA+Defs::SPACE);

        }else{

            result = tmpJS.toString();
        }
    }
    return result;
}

QPair<QString, QString> SqlAdapter::values(const QJSValue &query)
{
    QPair<QString, QString> result;

    if(query.hasProperty("values")){
        QJSValue valJS = query.property("values");

        if(valJS.isObject()){
            QStringList fieldsList;
            QStringList valuesList;

            QJSValueIterator it(valJS);
            while (it.hasNext()) {
                it.next();
                fieldsList<<formatField(it.name());
                valuesList<<formatValue(it.value().toVariant());
            }
            result.first = fieldsList.join(',');
            result.second = valuesList.join(',');

        }else{
            result.second = valJS.toString();
        }

    }
    return result;
}

QString SqlAdapter::returning(const QJSValue &query)
{
    QString result;

    if(query.hasProperty("returning")){
        QJSValue tmpJS = query.property("returning");
        if(tmpJS.isArray()){
            QStringList tmpAr;
            const int length = tmpJS.property("length").toInt();
            for (int i = 0; i < length; ++i) {
                tmpAr<<formatTable(tmpJS.property(i));
            }
            result = tmpAr.join(Defs::COMMA);
        }else
            result = tmpJS.toString();

        result = u" RETURNING %1"_qs.arg(result);
    }
    return result;
}
