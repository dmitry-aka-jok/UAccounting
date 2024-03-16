#include <QObject>
#include <QTest>

#include "sqldatabase.h"
#include "datapipe.h"
#include "asqltablemodel.h"
#include "guitablemodel.h"

class tst_guitablemodel : public QObject
{
    Q_OBJECT


private slots:
    void initTestCase()
    {
        Datapipe::instance()->setVariable(Defs::DEBUG_LEVEL, 2);
        SqlDatabase *sql = Datapipe::instance()->sql();

        QJSEngine eng;

        QJSValue structure = eng.newObject();

        QJSValue fka = eng.newArray();
        QJSValue pka = eng.newArray();

        QJSValue fks0 = eng.newObject();
        fks0.setProperty("field", "goodsgroup_id");
        fks0.setProperty("foreign_key", "id");
        fks0.setProperty("foreign_table", "goodsgroups");
        fks0.setProperty("table", "goods");
        fka.setProperty(0, fks0);

        QJSValue pks0 = eng.newObject();
        pks0.setProperty("field", "id");
        pks0.setProperty("table", "goods");
        pka.setProperty(0, pks0);
        QJSValue pks1 = eng.newObject();
        pks1.setProperty("field", "id");
        pks1.setProperty("table", "goodsgroups");
        pka.setProperty(1, pks1);

        structure.setProperty("foreign_keys", fka);
        structure.setProperty("primary_keys", pka);

        sql->setStructure(structure);

        sql->setDriver("QSQLITE");
        sql->setDatabase(":memory:");

        QScopedPointer<SqlQuery> qry(sql->query());

        qry->execute("CREATE TABLE [goodsgroups] (\
                         [id] INTEGER PRIMARY KEY,\
                         [name] TEXT,\
                         [remark] TEXT\
                     );");

        qry->execute("CREATE TABLE [goods] (\
                         [id] INTEGER PRIMARY KEY,\
                         [name] TEXT,\
                         [goodsgroup_id] INTEGER,\
                         [is_weight] INTEGER,\
                         [price_buy] FLOAT,\
                         [price_sell] FLOAT,\
                         [remark] TEXT,\
                         FOREIGN KEY([goodsgroup_id]) REFERENCES [goodsgroups]([id])\
                     );");

        qry->insert("insert into goodsgroups (id, name) values (1, 'group 1')");
        qry->insert("insert into goodsgroups (id, name) values (2, 'group 2')");
        qry->insert("insert into goodsgroups (id, name) values (3, 'group 3')");
    }


    void tst_01_simple()
    {
        SqlDatabase *sql = Datapipe::instance()->sql();
        QScopedPointer<SqlQuery> qry(sql->query());

        qry->remove("from goods");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (1, 'goods 1', 1)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (2, 'woods 3', 2)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (3, 'ads 4', 3)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (4, 'goods 2', 1)");

        ASqlTableModel model;

        model.setTable("goods");

        model.setFields(QVariantList{
                                     QVariantMap{{ASqlTableModel::FIELD, "name"}},
                                     QVariantMap{
                                         {ASqlTableModel::FIELD, "goodsgroup_id"},
                                         {ASqlTableModel::TYPE, ColumnType::LookupColumn},
                                         {ASqlTableModel::FOREIGN_TABLE, "goodsgroups"},
                                         {ASqlTableModel::FOREIGN_KEY, "id"},
                                         {ASqlTableModel::FOREIGN_NAME, "name"},
                                         },
                                     }
                        );
        QCOMPARE(model.select(), true);
        // key column will be added automatically
        QCOMPARE(model.columnCount(), 4);
        QCOMPARE(model.rowCount(), 4);

        GuiTableModel guimodel;

        QSqlRecord rec = model.record();

        QMap<int, int>redirectFromSource;
        // calculate
        for(int i=0, cnt=model.fields().count(); i<cnt; i++ ){
            int redirected = -1;

            QVariantMap fieldMap = model.fields().at(i).toMap();
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

        guimodel.setFields(model.fields());
        guimodel.setRedirections(redirectFromSource);
        guimodel.setSourceModel(&model);
        QCOMPARE(guimodel.columnCount(), 2);
        QCOMPARE(guimodel.rowCount(), 4);

        QCOMPARE(guimodel.lastColumn(), 1);
        QCOMPARE(guimodel.lastRow(), 3);

        QCOMPARE(guimodel.data(guimodel.index(0,0),ModelRoles::Data), "goods 1");
        QCOMPARE(guimodel.data(guimodel.index(0,1),ModelRoles::Data), "group 1");

        QCOMPARE(guimodel.data(guimodel.index(1,0),ModelRoles::Data), "woods 3");
        QCOMPARE(guimodel.data(guimodel.index(1,1),ModelRoles::Data), "group 2");

        QCOMPARE(guimodel.data(guimodel.index(2,0),ModelRoles::Data), "ads 4");
        QCOMPARE(guimodel.data(guimodel.index(2,1),ModelRoles::Data), "group 3");

        QCOMPARE(guimodel.data(guimodel.index(3,0),ModelRoles::Data), "goods 2");
        QCOMPARE(guimodel.data(guimodel.index(3,1),ModelRoles::Data), "group 1");

        guimodel.sort(1, Qt::AscendingOrder);

        QCOMPARE(guimodel.data(guimodel.index(0,0),ModelRoles::Data), "ads 4");
        QCOMPARE(guimodel.data(guimodel.index(0,1),ModelRoles::Data), "group 3");

        QCOMPARE(guimodel.data(guimodel.index(1,0),ModelRoles::Data), "goods 1");
        QCOMPARE(guimodel.data(guimodel.index(1,1),ModelRoles::Data), "group 1");

        QCOMPARE(guimodel.data(guimodel.index(2,0),ModelRoles::Data), "goods 2");
        QCOMPARE(guimodel.data(guimodel.index(2,1),ModelRoles::Data), "group 1");

        QCOMPARE(guimodel.data(guimodel.index(3,0),ModelRoles::Data), "woods 3");
        QCOMPARE(guimodel.data(guimodel.index(3,1),ModelRoles::Data), "group 2");

        guimodel.setFilter({0,"goods"});

        QCOMPARE(guimodel.data(guimodel.index(0,0),ModelRoles::Data), "goods 1");
        QCOMPARE(guimodel.data(guimodel.index(0,1),ModelRoles::Data), "group 1");

        QCOMPARE(guimodel.data(guimodel.index(1,0),ModelRoles::Data), "goods 2");
        QCOMPARE(guimodel.data(guimodel.index(1,1),ModelRoles::Data), "group 1");

        QCOMPARE(guimodel.columnCount(), 2);
        QCOMPARE(guimodel.rowCount(), 2);

        guimodel.sort(1, Qt::DescendingOrder);

        QCOMPARE(guimodel.data(guimodel.index(0,0),ModelRoles::Data), "goods 2");
        QCOMPARE(guimodel.data(guimodel.index(0,1),ModelRoles::Data), "group 1");

        QCOMPARE(guimodel.data(guimodel.index(1,0),ModelRoles::Data), "goods 1");
        QCOMPARE(guimodel.data(guimodel.index(1,1),ModelRoles::Data), "group 1");

     }


};

QTEST_MAIN(tst_guitablemodel)
#include "tst_guitablemodel.moc"
