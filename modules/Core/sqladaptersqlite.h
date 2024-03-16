#ifndef SQLADAPTERSQLITE_H
#define SQLADAPTERSQLITE_H

#include "sqladapter.h"

class SqlAdapterSqlite : public SqlAdapter
{
public:
    SqlAdapterSqlite();
    bool validate() override;

    virtual QString replace(const QJSValue &query) override;
//    virtual QString replace(const QJSValue &query);
};

#endif // SQLADAPTERSQLITE_H
