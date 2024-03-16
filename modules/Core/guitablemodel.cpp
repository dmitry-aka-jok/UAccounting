#include "guitablemodel.h"

#include <QDateTime>

GuiTableModel::GuiTableModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    m_serviceColumn(false), m_actionColumn(false), m_totalsRow(false), m_appendRow(false),
    m_filter{-1,""}
    ,m_lastRow(-1), m_lastColumn(-1)
{

    setDynamicSortFilter(false);

    connect(this, &GuiTableModel::filterChanged, [=](){invalidateRowsFilter(); recalcLastRow();});
    connect(this, &GuiTableModel::fieldsChanged, [=](){recalcLastColumn();});

    connect(this, &GuiTableModel::actionColumnChanged,  [=](){ if(m_actionColumn) m_lastColumn++; else m_lastColumn--;});
    connect(this, &GuiTableModel::serviceColumnChanged, [=](){ if(m_serviceColumn) m_lastColumn++; else m_lastColumn--;});

    connect(this, &GuiTableModel::totalsRowChanged, [=](){ if(m_totalsRow) m_lastRow++; else m_lastRow--;});
    connect(this, &GuiTableModel::appendRowChanged, [=](){ if(m_totalsRow) m_lastRow++; else m_lastRow--;});
}


QModelIndex GuiTableModel::index(int row, int column, const QModelIndex& parent) const {
    //int rc = rowCount();
    //int cc = columnCount();

    QModelIndex indx;
    if(
        (row==m_lastRow   && (m_appendRow || m_totalsRow))
        ||
        (row==m_lastRow-1 && (m_appendRow && m_totalsRow))
        ||
        (column==m_lastColumn && (m_actionColumn))
        ){
        indx = QSortFilterProxyModel::createIndex(row, column);
    }
    else
        indx = QSortFilterProxyModel::index(row, column, parent);

    return indx;
}

QModelIndex GuiTableModel::parent(const QModelIndex &index) const {
    //int rc = rowCount();
    QModelIndex indx;
    if( (index.row()==m_lastRow   && m_appendRow)
        ||
        (index.row()==m_lastRow-1 && m_appendRow && m_totalsRow)){
        indx = QModelIndex();
    }
    else
        indx = QSortFilterProxyModel::parent(index);

    return indx;
}

QModelIndex GuiTableModel::mapFromSource(const QModelIndex &source) const
{
    if (!source.isValid())
        return QModelIndex();


    int redirect = redirectFromSource.value(source.column(), -1) + (m_serviceColumn?1:0);

    QModelIndex indx = QSortFilterProxyModel::mapFromSource(source).siblingAtColumn(redirect);

    return indx;
}

QModelIndex GuiTableModel::mapToSource(const QModelIndex &proxy) const
{
    if (!proxy.isValid())
        return QModelIndex();


    if(m_serviceColumn && proxy.column()==0)
        return QModelIndex();

    if(m_actionColumn && proxy.column()==columnCount()-1)
        return QModelIndex();

    int redirect = redirectToSource.value(proxy.column() - (m_serviceColumn?1:0), -1);

    QModelIndex indx = QSortFilterProxyModel::mapToSource(proxy).siblingAtColumn(redirect);

    return indx;
}

QHash<int, QByteArray>GuiTableModel::roleNames() const
{
    return {{ModelRoles::Data, "Data"}
        ,{ModelRoles::Type, "Type"}
        ,{ModelRoles::Digits, "Digits"}
        ,{ModelRoles::FieldName, "FieldName"}
        ,{ModelRoles::SourceRow, "SourceRow"}
    };
}

int GuiTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    int cnt = m_fields.count() + (m_serviceColumn?1:0) + (m_actionColumn?1:0);
    return cnt;
}

int GuiTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    int rc = QSortFilterProxyModel::rowCount(parent);
    return rc + (m_appendRow?1:0) + (m_totalsRow?1:0);
}


void GuiTableModel::recalcLastColumn()
{
    int cc = columnCount();
    if(m_lastColumn!=cc-1){
        m_lastColumn = cc-1;
        emit lastColumnChanged();
    }
}

void GuiTableModel::recalcLastRow()
{
    int rc = rowCount();
    if(m_lastRow!=rc-1){
        m_lastRow = rc-1;
        emit lastRowChanged();
    }
}

QVariant GuiTableModel::data(const QModelIndex &index, int role)const
{
    if (role==ModelRoles::Type){
        // TODO totals column
        if (m_appendRow && index.row() == m_lastRow){
            if(index.column()==0)
                return ColumnType::AppendRow;
            else
                return ColumnType::UnknownColumn;
        }else{
            if (m_serviceColumn && index.column() == 0)
                return ColumnType::ServiceColumn;
            else
                if (m_actionColumn && index.column() ==m_lastColumn)
                    return ColumnType::ActionColumn;
                else{
                    return m_fields.at(index.column() - (m_serviceColumn?1:0))
                        .toMap().value(GuiTableModel::TYPE, ColumnType::StringColumn);
                }
        }
    }

    if (role==ModelRoles::Data){
        if (m_appendRow && index.row() == m_lastRow){
            return QString();
        }else{
            if (m_serviceColumn && index.column() == 0){
                return QString();
            }else
                if (m_actionColumn && index.column() == m_lastColumn){
                    return QString();
                }else{
                    return QSortFilterProxyModel::data(index, Qt::DisplayRole);
                }
        }
    }

    if (role==ModelRoles::Digits){
        if (m_appendRow && index.row() == m_lastRow){
            return 0;
        }else{
            if (m_serviceColumn && index.column() == 0)
                return 0;
            else
                if (m_actionColumn && index.column() ==m_lastColumn)
                    return 0;
                else{
                    return m_fields.at(index.column() - (m_serviceColumn?1:0))
                        .toMap().value(GuiTableModel::DIGITS, 0);
                }
        }
    }

    if (role==ModelRoles::FieldName){
        if (m_appendRow && index.row() == m_lastRow){
            return QString();
        }else{
            if (m_serviceColumn && index.column() == 0)
                return QString();
            else
                if (m_actionColumn && index.column() ==m_lastColumn)
                    return QString();
                else{
                    return m_fields.at(index.column() - (m_serviceColumn?1:0))
                        .toMap().value(GuiTableModel::FIELD, 0);
                }
        }
    }

    if (role==ModelRoles::SourceRow){
        return mapToSource(index).row();
    }

    return QVariant();
}

QString GuiTableModel::columnHeaderName(int column) const
{
    if (m_serviceColumn && column == 0)
        return QString();
    else
        if (m_actionColumn && column ==m_lastColumn)
            return QString();
        else{
            return m_fields.at(column - (m_serviceColumn?1:0))
                .toMap().value(GuiTableModel::NAME, 0).toString();
        }

    return QString();
}

void GuiTableModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);

    connect(sourceModel, &QAbstractItemModel::columnsInserted, this, [=](){recalcLastColumn();});
    connect(sourceModel, &QAbstractItemModel::columnsRemoved, this, [=](){recalcLastColumn();});
    connect(sourceModel, &QAbstractItemModel::rowsInserted, this, [=](){recalcLastRow();});
    connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, [=](){recalcLastRow();});

    recalcLastColumn();
    recalcLastRow();// row will be recalced by by filter but this is not always
}


bool GuiTableModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(m_filter.second.isEmpty())
        return true;


    for(int i=0, cnt=columnCount(); i<cnt; i++){

        if(m_filter.first >= 0 && i != m_filter.first)
            continue;


        QModelIndex index = sourceModel()->index(sourceRow, redirectToSource.value(i), sourceParent);
        QString data = sourceModel()->data(index, Qt::DisplayRole).value<QString>();

        if(data.contains(m_filter.second, Qt::CaseInsensitive))
            return true;
    }

    return false;
}

bool GuiTableModel::filterAcceptsColumn(int source_column, const QModelIndex &sourceParent) const
{
    return QSortFilterProxyModel::filterAcceptsColumn(source_column, sourceParent);
}

bool GuiTableModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    //ColumnType::Value type = static_cast<ColumnType::Value> (data(left, ModelRoles::Type).toInt());
    ColumnType::Value type = static_cast<ColumnType::Value> (
        m_fields.at(left.column() - (m_serviceColumn?1:0))
            .toMap().value(GuiTableModel::TYPE, ColumnType::StringColumn).toInt());


    if(type==ColumnType::DateColumn){
        return sourceModel()->data(left).toDate() < sourceModel()->data(right).toDate();
    }
    if(type==ColumnType::TimeColumn){
        return sourceModel()->data(left).toTime() < sourceModel()->data(right).toTime();
    }
    if(type==ColumnType::DateTimeColumn){
        return sourceModel()->data(left).toDateTime() < sourceModel()->data(right).toDateTime();
    }
    if(type==ColumnType::PKColumn || type==ColumnType::FKColumn || type==ColumnType::IntColumn){
        return sourceModel()->data(left).toInt() < sourceModel()->data(right).toInt();
    }
    if(type==ColumnType::StringColumn){
        return sourceModel()->data(left).toString() < sourceModel()->data(right).toString();
    }
    if(type==ColumnType::BoolColumn){
        return sourceModel()->data(left).toBool() < sourceModel()->data(right).toBool();
    }
    if(type==ColumnType::NumericColumn){
        return sourceModel()->data(left).toDouble() < sourceModel()->data(right).toDouble();
    }


    return sourceModel()->data(left).toString() < sourceModel()->data(right).toString();
}


int GuiTableModel::sourceRow(int row)
{
    return mapToSource(index(row, (m_serviceColumn?1:0))).row();
}

void GuiTableModel::setRedirections(QMap<int, int> redirections)
{
    redirectFromSource = redirections;
    redirectToSource.clear();
    for(auto it=redirectFromSource.constBegin(); it!=redirectFromSource.constEnd(); ++it)
        redirectToSource.insert(it.value(), it.key());
}


