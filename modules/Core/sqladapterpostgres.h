#ifndef SQLADAPTERPOSTGRES_H
#define SQLADAPTERPOSTGRES_H

#include "sqladapter.h"

class SqlAdapterPostgres : public SqlAdapter
{
public:
    SqlAdapterPostgres();
    bool validate() override;

    QString replace(const QJSValue &query) override;
};

#endif // SQLADAPTERPOSTGRES_H
