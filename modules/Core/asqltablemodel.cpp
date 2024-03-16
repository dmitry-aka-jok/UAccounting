#include "asqltablemodel.h"

#include "defs.h"
#include "datapipe.h"
#include "guitablemodel.h"
#include "datapipe.h"

ASqlTableModel::ASqlTableModel(QObject *parent)
    : QAbstractTableModel{parent}
{
}

bool ASqlTableModel::submit()
{
    SqlDatabase *sql = Datapipe::instance()->sql(m_connectionName);
    QString pk = sql->primaryKey(m_table);

    QScopedPointer<SqlQuery> query(sql->query());

    QJSEngine jse;

    auto upd = m_updatedRecords.constBegin();
    for (;upd != m_updatedRecords.constEnd(); ++upd) {
        if(m_insertedRecords.contains(upd.key()))
            continue;
        if(m_deletedRecords.contains(upd.key()))
            continue;

        QJSValue updateFields = jse.newObject();
        QJSValue keyFields = jse.newObject();

        QSqlRecord recBefore = m_records.value(upd.key());
        QSqlRecord recCurrent= upd.value();
        for(int i = 0, cnt = recBefore.count(); i < cnt; ++i){
            if(m_fieldTypes.at(i)==ColumnType::LookupColumn)
                continue;

            if(recBefore.value(i) != recCurrent.value(i)){
                updateFields.setProperty(recBefore.fieldName(i),
                                         jse.toScriptValue(recCurrent.value(i)));
            }

            if(recBefore.fieldName(i)==pk)
                keyFields.setProperty(pk,  jse.toScriptValue(recBefore.value(i))); // recBefore - for old ID
        }
        QJSValue jsquery = jse.newObject();
        jsquery.setProperty("table", m_table);
        jsquery.setProperty("set", updateFields);
        jsquery.setProperty("where", keyFields);

        if(!query->update(jsquery)){
            m_lastError = query->lastError();
            m_lastQuery = query->lastQuery();
            return false;
        }

        m_records[upd.key()] = recCurrent;
    }

    int alreadyDeleted=0;
    auto del = m_deletedRecords.constBegin();

    for (;del != m_deletedRecords.constEnd(); ++del) {
        if(m_insertedRecords.contains(del.key()))
            continue;

        QJSValue keyFields = jse.newObject();

        QSqlRecord recBefore = m_records.value(del.key() - alreadyDeleted);
        for(int i = 0, cnt = recBefore.count(); i < cnt; ++i){
            if(recBefore.fieldName(i)==pk)
               keyFields.setProperty(pk, jse.toScriptValue(recBefore.value(i))); // recBefore - for old ID
        }

        QJSValue jsquery = jse.newObject();
        jsquery.setProperty("table", m_table);
        jsquery.setProperty("where", keyFields);

        if(!query->remove(jsquery)){
            m_lastError = query->lastError();
            m_lastQuery = query->lastQuery();
            return false;
        }

        m_records.removeAt(del.key() - alreadyDeleted);

        alreadyDeleted++;
    }

    auto ins = m_insertedRecords.constBegin();
    for (;ins != m_insertedRecords.constEnd(); ++ins) {
        if(m_deletedRecords.contains(ins.key()))
            continue;
        QJSValue insertFields = jse.newObject();
        QString pkField;
        QSqlRecord recCurrent;
        if(m_updatedRecords.contains(ins.key()))
            recCurrent = m_updatedRecords.value(ins.key());
        else
            recCurrent = ins.value();

        for(int i = 0, cnt = recCurrent.count(); i < cnt; ++i){
            if(m_fieldTypes.at(i)==ColumnType::LookupColumn)
                continue;
            if(m_fieldTypes.at(i)==ColumnType::PKColumn){
                pkField=recCurrent.fieldName(i);
                continue;
            }

            insertFields.setProperty(recCurrent.fieldName(i),
                                     jse.toScriptValue(recCurrent.value(i)));
        }
        QJSValue jsquery = jse.newObject();
        jsquery.setProperty("table", m_table);
        jsquery.setProperty("values", insertFields);
        if(!pkField.isEmpty())
            jsquery.setProperty("returning", pkField);

        if(!query->insert(jsquery)){
            m_lastError = query->lastError();
            m_lastQuery = query->lastQuery();
            return false;
        }
        QVariant pkValue = query->value(0);
        recCurrent.setValue(recCurrent.indexOf(pkField), pkValue);

        m_records.append(recCurrent);
    }


    m_deletedRecords.clear();
    m_insertedRecords.clear();
    m_updatedRecords.clear();

    return true;
}

void ASqlTableModel::revert()
{
    m_deletedRecords.clear();
    m_insertedRecords.clear();
    m_updatedRecords.clear();
}

Qt::ItemFlags ASqlTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);

    if (index.isValid())
        flags |= Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;

    return flags;
}

QVariant ASqlTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(m_records.isEmpty())
        return QVariant();


    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        // calc shift by deleted records
        int row = shiftDeleted(index.row());
        // search info in right container.
        if (m_updatedRecords.contains(row)){
            return m_updatedRecords.value(row).value(index.column());
        } else
            if (m_insertedRecords.contains(row)){
                return m_insertedRecords.value(row).value(index.column());
            } else
                return m_records.at(row).value(index.column());
    };

    return QVariant();
}

QVariant ASqlTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal) {
        return m_record.fieldName(section);
    }
    else {
        return section + 1;
    }

    return QVariant();
}

int ASqlTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_records.count() + m_insertedRecords.count() - m_deletedRecords.count();
}

int ASqlTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_record.count();
}

bool ASqlTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    int row = shiftDeleted(index.row());
    if(!m_updatedRecords.contains(row)){
        if(m_insertedRecords.contains(row))
            m_updatedRecords[row] = m_insertedRecords.value(row);
        else
            m_updatedRecords[row] = m_records.value(row);
    }
    m_updatedRecords[row].setValue(index.column(), value);
    m_updatedRecords[row].setGenerated(index.column(), true);

    emit dataChanged(index, index);
    return true;
}

bool ASqlTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int)
{
    return false;
}

bool ASqlTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);

    for (int i = 0; i < count; ++i) {
        m_insertedRecords.insert(row+i+m_deletedRecords.count(), m_record);
    }
    endInsertRows();

    return true;
}

bool ASqlTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
     beginRemoveRows(QModelIndex(), row, row + count - 1);
     for (int i = 0; i < count; ++i){
         // mark as deleted always start row - it will be correct  shifted
         int shifted = shiftDeleted(row);
         m_deletedRecords.insert(shifted, shifted); //(row+i);

     }
     endRemoveRows();

     return true;
}

QModelIndex ASqlTableModel::index(int row, int column, const QModelIndex &parent) const
{
    return QAbstractTableModel::index(row, column, parent);
}

QModelIndex ASqlTableModel::index(int row, const QString &columnName, const QModelIndex &parent) const
{
    int column = record().indexOf(columnName);
    return QAbstractTableModel::index(row, column, parent);
}

int ASqlTableModel::shiftDeleted(int row) const
{
    auto del = m_deletedRecords.constBegin();
    for(; del != m_deletedRecords.constEnd(); ++del){
        if(row >= del.key())
            row++;
        else
            break;
    }
    return row;
}

bool ASqlTableModel::isDirty() const
{
    if(!m_insertedRecords.isEmpty())
        return true;
    if(!m_updatedRecords.isEmpty())
        return true;
    if(!m_deletedRecords.isEmpty())
        return true;

    return false;
}

QSqlRecord ASqlTableModel::record() const
{
    return m_record;
}

QSqlRecord ASqlTableModel::record(int row) const
{
    int crow = shiftDeleted(row);
    if(!m_updatedRecords.contains(crow)){
        if(m_insertedRecords.contains(crow))
            return m_insertedRecords.value(crow);
        else
            return m_records.value(crow);
    }else
        return m_updatedRecords.value(crow);
}

QString ASqlTableModel::lastQuery() const
{
    return m_lastQuery;
}

QSqlError ASqlTableModel::lastError() const
{
    return m_lastError;
}

QVariantMap ASqlTableModel::rowMap(int row)
{
    QVariantMap res;
    QSqlRecord rec = record();

    for(int i=0, cnt = rec.count(); i < cnt; i++){
        QString field = rec.fieldName(i);

        res.insert(field, data(index(row, i), Qt::DisplayRole));
    }
    return res;
}


// QSqlDatabase ASqlTableModel::database()
// {
//     return Datapipe::instance()->sql(m_connectionName)->currentDatabase();
// }


QVariantMap ASqlTableModel::rowKey(int row)
{
    QVariantMap res;
    QSqlRecord rec = record();

    QStringList pks = Datapipe::instance()->sql(m_connectionName)->primaryKey(m_table).split(",");

    foreach(auto pk, pks){
        res.insert(pk, data(index(row, rec.indexOf(pk)), Qt::DisplayRole));
    }

    return res;
}

bool ASqlTableModel::select()
{

    m_records.clear();
    m_deletedRecords.clear();
    m_insertedRecords.clear();
    m_updatedRecords.clear();


    QStringList fieldsList;
    QStringList joinsList;

    SqlDatabase *sql = Datapipe::instance()->sql(m_connectionName);

    QString talias;
    int indasp = table().indexOf(Defs::SPACE);
    if(indasp>=0)
        talias = sql->formatAlias(table().mid(indasp+1));
    else
        talias = sql->formatAlias(table());


    QString pk = sql->primaryKey(table());
    if(!pk.isEmpty()){
        pk = u"%1.%2"_qs.arg(talias, pk);
        fieldsList.append(pk);
        m_fieldTypes<<ColumnType::PKColumn;
    }

    for(int i=0, cnt=fields().count(); i<cnt; i++ ){
        QVariantMap fieldMap = fields().at(i).toMap();
        QString field = fieldMap.value(ASqlTableModel::FIELD).toString();
        QString calcd  = fieldMap.value(ASqlTableModel::CALCULATED).toString();

        if(!calcd.isEmpty()){
            fieldsList.append(u"%1 AS %2"_qs.arg(calcd, field));
            m_fieldTypes<<ColumnType::DataColumn;

        }else{
            ColumnType::Value ftype = static_cast<ColumnType::Value>(fieldMap.value(ASqlTableModel::TYPE).toInt());
            if(ftype==ColumnType::LookupColumn){
                fieldsList.append(u"%1.%2"_qs.arg(talias, field));
                m_fieldTypes<<ColumnType::FKColumn<<ColumnType::LookupColumn;

                QString fkey    = fieldMap.value(ASqlTableModel::FOREIGN_KEY).toString();
                QString ftable  = fieldMap.value(ASqlTableModel::FOREIGN_TABLE).toString();
                QString fname   = fieldMap.value(ASqlTableModel::FOREIGN_NAME).toString();

                QString falias;
                int indsp = ftable.indexOf(Defs::SPACE);
                if(indsp>=0){
                    falias = sql->formatAlias(ftable.mid(indsp+1));
                    ftable = ftable.left(indsp);
                }
                else
                    falias = sql->formatAlias(ftable);

                fieldsList.append(u"%1.%2 AS %3_%4"_qs.arg(falias, fname, falias, fname));
                joinsList.append(u"left join %1 %2 on %3.%4 = %5.%6"_qs.arg(
                    ftable
                    ,falias
                    ,falias
                    ,fkey
                    ,talias
                    ,field
                    ));
            } else {
                fieldsList.append(u"%1.%2"_qs.arg(talias, field));
                m_fieldTypes<<ColumnType::DataColumn;
            }
        }
    }

    // Qt shield with quotas tablename with scheme in Postgres
    QString flt = filter();
    //    if(table().contains(".")){
    //      flt= flt.replace(table().replace(".","\".\""), talias);
    //    }
    //qDebug()<<fieldsList;

    QJSEngine jsengine;
    QJSValue jsquery = jsengine.newObject();
    jsquery.setProperty("table",table());
    jsquery.setProperty("alias",talias);
    jsquery.setProperty("fields",fieldsList.join(Defs::COMMA+Defs::SPACE));
    if(!joinsList.isEmpty())
        jsquery.setProperty("join",joinsList.join(Defs::SPACE));
    if(!flt.isEmpty())
        jsquery.setProperty("where",flt);
    if(!pk.isEmpty())
        jsquery.setProperty("order",pk);

    bool ok;
    QScopedPointer<SqlQuery> query(sql->query());
    query->select(jsquery);
    m_lastQuery = query->lastQuery();

    if(!query->isActive()){
        m_lastError = query->lastError();
        return false;
    }


    m_record = query->record();
    while(query->next()) {
        m_records << query->record();
    }

    return true;
}
