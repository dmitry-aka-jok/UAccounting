#include "guitable.h"

#include <QQmlEngine>

#include "datapipe.h"
#include "sqldatabase.h"
#include "theme.h"


GuiTable::GuiTable(QQuickItem *parent) : GuiItem(parent)
  ,m_sortField(0), m_sortOrder(0), m_filterColumn(-1)
  ,saveSortField(false), saveSortOrder(false), saveFilterColumn(false),saveColumnWidth(false)
  ,m_usage(GuiTableUsage::Table),m_editorType(GuiTableUsage::NoEditor)
  ,m_sqlTableModel(nullptr)
{
    connect(this, &GuiTable::filterColumnChanged,  [=](){saveFilterColumn = true; applyFilter();});
    connect(this, &GuiTable::filterStringChanged,  [=](){applyFilter();});
    connect(this, &GuiTable::sortFieldChanged,     [=](){saveSortField   = true; applySort();});
    connect(this, &GuiTable::sortOrderChanged,     [=](){saveSortOrder   = true; applySort();});

    connect(&m_guiTableModel, &GuiTableModel::dataChanged, this, [=](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles ){
        // TODO Check is this bug in qml tableview - not redraw all cells
        //qDebug()<<"data changed"<<topLeft<<bottomRight<<roles;

        int cc = m_guiTableModel.columnCount();
        if(topLeft.column()==0 && bottomRight.column()==cc-1)
            return;

        QModelIndex indexS = topLeft.siblingAtColumn(0);
        QModelIndex indexF = bottomRight.siblingAtColumn(cc-1);

        emit m_guiTableModel.dataChanged(indexS, indexF);
    });

    serviceColumnRegister();
    actionColumnRegister();
    totalsRowRegister();
    appendRowRegister();
}

GuiTable::~GuiTable()
{
    commit();

    saveSettings();
}

void GuiTable::commit()
{
/*
    if(m_sqlTableModel.isDirty() && m_editorType == GuiTableUsage::InRow)
        m_sqlTableModel.submit();
*/
}

void GuiTable::init()
{
    if(m_table.isEmpty())
        return;
    if(m_fields.isEmpty())
        return;

    m_sqlTableModel.setConnectionName(m_database);
    m_sqlTableModel.setTable(m_table);

    // TODO make list conditions
    if(m_conditions.isValid())
        m_sqlTableModel.setFilter(m_conditions.toString());

    if(m_usage==GuiTableUsage::SubTable){
        auto fk = Datapipe::instance()->sql(m_database)->foreignKey(m_table, m_mainTable);
    }

    m_sqlTableModel.setFields(m_fields);

    if (!m_sqlTableModel.select()){
        // Messages::instance()->addMessage(MessageType::CriticalError, __PRETTY_FUNCTION__,
        //                                  u"%1\n%2\n%3"_qs.arg(
        //                                      m_sqlTableModel.lastError().driverText()
        //                                      ,m_sqlTableModel.lastError().databaseText()
        //                                      ,m_sqlTableModel.lastQuery())
        //                                  );
        qCritical()<<m_sqlTableModel.lastError().driverText();
        qCritical()<<m_sqlTableModel.lastError().databaseText();
        qCritical()<<m_sqlTableModel.lastQuery();
    }

    m_guiTableModel.setSourceModel(&m_sqlTableModel);

    QSqlRecord rec = m_sqlTableModel.record();

    redirectFromSource.clear();
    // calculate
    for(int i=0, cnt=m_fields.count(); i<cnt; i++ ){
        int redirected = -1;

        QVariantMap fieldMap = m_fields.at(i).toMap();
        QString fieldNeed = fieldMap.value(ASqlTableModel::FIELD).toString();

        // try to find in record to redirect
        for(int j=0, cntj = rec.count(); j<cntj; j++){
            if (rec.fieldName(j)==fieldNeed){
                if (fieldMap.value(ASqlTableModel::FOREIGN_KEY).isValid())
                    redirected = j+1; // next field
                else
                    redirected = j;

                break;
            }
        }

        if(redirected>=0)
            redirectFromSource.insert(redirected, i);
    }

    redirectToSource.clear();
    for (auto i = redirectFromSource.cbegin(), end = redirectFromSource.cend(); i != end; ++i)
        redirectToSource.insert(i.value(), i.key());

    m_guiTableModel.setFields(m_fields);
    m_guiTableModel.setRedirections(redirectFromSource);

    loadSettings();

    applyFilter();
    applySort();
}

GuiTableModel *GuiTable::guiTableModel()
{
    return &m_guiTableModel;
}

ASqlTableModel *GuiTable::sqlTableModel()
{
    return &m_sqlTableModel;
}


void GuiTable::applySort()
{
    if(m_sortField < m_guiTableModel.columnCount())
    {
        if (m_sortOrder==0)
            m_guiTableModel.sort(-1, Qt::AscendingOrder);
        if (m_sortOrder==1)
            m_guiTableModel.sort(redirectToSource.value(m_sortField,-1), Qt::AscendingOrder);
        if (m_sortOrder==2)
            m_guiTableModel.sort(redirectToSource.value(m_sortField,-1), Qt::DescendingOrder);
    }
}

void GuiTable::applyFilter()
{
    m_guiTableModel.setFilter({m_filterColumn,m_filterString});
}

void GuiTable::loadSettings()
{
    /*
    Datapipe *datapipe = Datapipe::instance();
    QScopedPointer<SqlQuery> query (datapipe->sql(database())->query());

    QString modifier;
    if(m_execStack.contains(u"onPickerRequest@"_qs))
        modifier = u"@onPickerRequest"_qs;
    if(m_execStack.contains(u"startEditor@"_qs))
        modifier = u"@startEditor"_qs;

    QJSEngine engine;
    QJSValue jsquery = engine.newObject();
    jsquery.setProperty("table", Defs::SETTINGS_TABLE);
    jsquery.setProperty("fields", Defs::SETTINGS_VALUE);
    QJSValue jswhere = engine.newObject();
    jswhere.setProperty(Defs::SETTINGS_SECTION, table());
    jswhere.setProperty(Defs::SETTINGS_OPTION, "columnsWidth"+modifier);
    jswhere.setProperty(Defs::SETTINGS_USER, datapipe->variable("user",-1).toInt());
    jsquery.setProperty("where", jswhere);

    query->select(jsquery);

    m_columnWidths.clear();
    if(query->next()){
        QStringList widths = query->value(0).toString().split(",");

        for(int wi=0, wcnt=widths.count(); wi<wcnt; ++wi ){
            QString fieldt = widths.at(wi);
            QString field = fieldt.left(fieldt.indexOf(Defs::EQ));
            int fsize = fieldt.mid(fieldt.indexOf(Defs::EQ)+1).toInt();

            for(int fi=0, fcnt=fields().count(); fi<fcnt; ++fi){
                QVariantMap fld = fields().at(fi).toMap();
                if (fld.value(Defs::FIELD).toString() == field){
                    m_columnWidths.insert(fi, fsize);
                    break;
                }
            }
        }
    }

    query.select(QVariantMap({
                                 {"table",Defs::TABLE_SETTINGS},
                                 {"fields","value"},
                                 {"where", QVariantMap({
                                      {"section", m_table},
                                      {"option","filterColumn"+modifier},
                                      {"user_id", datapipe->variable("user_id",-1)}}
                                  )}
                             }));
    m_filterColumn = -1;
    if(query.next())
        m_filterColumn = query.value(0).toInt();

    query.select(QVariantMap({
                                 {"table",Defs::TABLE_SETTINGS},
                                 {"fields","value"},
                                 {"where", QVariantMap({
                                      {"section", m_table},
                                      {"option","sortField"+modifier},
                                      {"user_id", datapipe->variable("user_id",-1)}}
                                  )}
                             }));
    m_sortField = 0;
    if(query.next())
        m_sortField = query.value(0).toInt();

    query.select(QVariantMap({
                                 {"table",Defs::TABLE_SETTINGS},
                                 {"fields","value"},
                                 {"where", QVariantMap({
                                      {"section", m_table},
                                      {"option","sortOrder"+modifier},
                                      {"user_id", datapipe->variable("user_id",-1)}}
                                  )}
                             }));
    m_sortField = 0;
    if(query.next())
        m_sortOrder = query.value(0).toInt();
*/
}



void GuiTable::saveSettings() {

    Datapipe *datapipe = Datapipe::instance();
    //Sql *sql = Datapipe::instance()->sql(m_database);
    //QSqlDatabase db = datapipe->sql(database())->currentDatabase();
    QScopedPointer<SqlQuery> query (datapipe->sql(database())->query());
    qDebug()<<database()<<database().isEmpty();


    QString modifier;
    if(m_execStack.contains(u"onPickerRequest@"_qs))
        modifier = u"@onPickerRequest"_qs;
    if(m_execStack.contains(u"startEditor@"_qs))
        modifier = u"@startEditor"_qs;

    QStringList vals;
    foreach(int fieldNo, m_columnWidths.keys()){
        QString field = m_fields.value(fieldNo).toMap().value(ASqlTableModel::FIELD).toString();
        vals.append(u"%1=%2"_qs.arg(field).arg(m_columnWidths.value(fieldNo)));
    }

    QJSEngine engine;
    QJSValue jsquery = engine.newObject();
    jsquery.setProperty("table", Defs::SETTINGS_TABLE);
    QJSValue jsflds = engine.newObject();
    jsflds.setProperty(Defs::SETTINGS_SECTION, table());
    jsflds.setProperty(Defs::SETTINGS_USER, datapipe->variable("user",-1).toInt());
    jsflds.setProperty(Defs::SETTINGS_OPTION, "columnsWidth"+modifier);
    jsflds.setProperty(Defs::SETTINGS_VALUE, vals.join(Defs::COMMA));
    jsquery.setProperty("values", jsflds);
    qDebug()<<jsquery.toVariant();
    query->replace(jsquery);

    jsflds.setProperty(Defs::SETTINGS_OPTION, "filterColumn"+modifier);
    jsflds.setProperty(Defs::SETTINGS_VALUE, m_filterColumn);
    jsquery.setProperty("values", jsflds);
    query->replace(jsquery);

    jsflds.setProperty(Defs::SETTINGS_OPTION, "sortField"+modifier);
    jsflds.setProperty(Defs::SETTINGS_VALUE, m_sortField);
    jsquery.setProperty("values", jsflds);
    query->replace(jsquery);

    jsflds.setProperty(Defs::SETTINGS_OPTION, "sortOrder"+modifier);
    jsflds.setProperty(Defs::SETTINGS_VALUE, m_sortOrder);
    jsquery.setProperty("values", jsflds);
    query->replace(jsquery);
}

int GuiTable::columnWidth(int column, const QString &type)
{
    int mult = 1;
    if(type==u"string"_qs)
        mult = 5;
    if(type==u"numeric"_qs)
        mult = 3;

    return m_columnWidths.value(column, mult * Theme::instance()->BASE_WIDTH());
}

void GuiTable::storeColumnWidth(int column, int width)
{
    m_columnWidths.insert(column, width);
}


