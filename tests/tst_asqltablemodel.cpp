#include <QObject>
#include <QTest>

#include "sqldatabase.h"
#include "datapipe.h"
#include "asqltablemodel.h"
#include "guitablemodel.h"

class tst_asqltablemodel : public QObject
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
        qry->insert("insert into goods (id, name, goodsgroup_id) values (2, 'goods 2', 2)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (3, 'goods 3', 3)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (4, 'goods 4', 1)");


        ASqlTableModel model;

        model.setTable("goods");
        model.setFields(QVariantList{
                                     QVariantMap{{ASqlTableModel::FIELD, "name"}},
                                     QVariantMap{{ASqlTableModel::FIELD, "goodsgroup_id"}},
                                     }
                        );


        QCOMPARE(model.select(), true);
        qDebug()<<model.lastQuery();
        // key column will be added automatically
        QCOMPARE(model.columnCount(), 3);
        QCOMPARE(model.rowCount(), 4);

        QCOMPARE(model.data(model.index(0,0)), 1);
        QCOMPARE(model.data(model.index(0,1)), "goods 1");

        QCOMPARE(model.data(model.index(1,0)), 2);
        QCOMPARE(model.data(model.index(1,1)), "goods 2");

        QCOMPARE(model.data(model.index(2,0)), 3);
        QCOMPARE(model.data(model.index(2,1)), "goods 3");

        QCOMPARE(model.data(model.index(3,0)), 4);
        QCOMPARE(model.data(model.index(3,1)), "goods 4");

     }


    void tst_02_lookup()
    {
        SqlDatabase *sql = Datapipe::instance()->sql();
        QScopedPointer<SqlQuery> qry(sql->query());

        qry->remove("from goods");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (1, 'goods 1', 1)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (2, 'goods 2', 2)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (3, 'goods 3', 3)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (4, 'goods 4', 1)");

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
        // key and lookup columns will be added automatically
        QCOMPARE(model.columnCount(), 4);
        QCOMPARE(model.rowCount(), 4);

        QCOMPARE(model.data(model.index(0,0)), 1);
        QCOMPARE(model.data(model.index(0,1)), "goods 1");
        QCOMPARE(model.data(model.index(0,2)), 1);
        QCOMPARE(model.data(model.index(0,3)), "group 1");

        QCOMPARE(model.data(model.index(1,0)), 2);
        QCOMPARE(model.data(model.index(1,1)), "goods 2");
        QCOMPARE(model.data(model.index(1,2)), 2);
        QCOMPARE(model.data(model.index(1,3)), "group 2");

        QCOMPARE(model.data(model.index(2,0)), 3);
        QCOMPARE(model.data(model.index(2,1)), "goods 3");
        QCOMPARE(model.data(model.index(2,2)), 3);
        QCOMPARE(model.data(model.index(2,3)), "group 3");

        QCOMPARE(model.data(model.index(3,0)), 4);
        QCOMPARE(model.data(model.index(3,1)), "goods 4");
        QCOMPARE(model.data(model.index(3,2)), 1);
        QCOMPARE(model.data(model.index(3,3)), "group 1");

     }

    void tst_03_update()
    {
        SqlDatabase *sql = Datapipe::instance()->sql();
        QScopedPointer<SqlQuery> qry(sql->query());

        qry->remove("from goods");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (1, 'goods 1', 1)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (2, 'goods 2', 2)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (3, 'goods 3', 3)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (4, 'goods 4', 1)");

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
        //    {field:"goodsgroup_id", name:"Група", type:GuiTableModel.LookupColumn, foreign_table:"store.goodsgroups", foreign_key:"id", foreign_name:"name"},

            QCOMPARE(model.select(), true);

        // check pure changing
        model.setData(model.index(0,1), "supergoods 1");
        model.setData(model.index(0,2), 2);
        model.setData(model.index(2,1), "supergoods 3");
        model.setData(model.index(2,2), 2);

        QCOMPARE(model.columnCount(), 4);
        QCOMPARE(model.rowCount(), 4);

        QCOMPARE(model.data(model.index(0,0)), 1);
        QCOMPARE(model.data(model.index(0,1)), "supergoods 1");
        QCOMPARE(model.data(model.index(0,2)), 2);
        QCOMPARE(model.data(model.index(0,3)), "group 1"); //didn`t change auto

        QCOMPARE(model.data(model.index(1,0)), 2);
        QCOMPARE(model.data(model.index(1,1)), "goods 2");
        QCOMPARE(model.data(model.index(1,2)), 2);
        QCOMPARE(model.data(model.index(1,3)), "group 2");

        QCOMPARE(model.data(model.index(2,0)), 3);
        QCOMPARE(model.data(model.index(2,1)), "supergoods 3");
        QCOMPARE(model.data(model.index(2,2)), 2);
        QCOMPARE(model.data(model.index(2,3)), "group 3");  //didn`t change auto

        QCOMPARE(model.data(model.index(3,0)), 4);
        QCOMPARE(model.data(model.index(3,1)), "goods 4");
        QCOMPARE(model.data(model.index(3,2)), 1);
        QCOMPARE(model.data(model.index(3,3)), "group 1");

        model.revert();

        QCOMPARE(model.columnCount(), 4);
        QCOMPARE(model.rowCount(), 4);

        QCOMPARE(model.data(model.index(0,0)), 1);
        QCOMPARE(model.data(model.index(0,1)), "goods 1");
        QCOMPARE(model.data(model.index(0,2)), 1);
        QCOMPARE(model.data(model.index(0,3)), "group 1");

        QCOMPARE(model.data(model.index(1,0)), 2);
        QCOMPARE(model.data(model.index(1,1)), "goods 2");
        QCOMPARE(model.data(model.index(1,2)), 2);
        QCOMPARE(model.data(model.index(1,3)), "group 2");

        QCOMPARE(model.data(model.index(2,0)), 3);
        QCOMPARE(model.data(model.index(2,1)), "goods 3");
        QCOMPARE(model.data(model.index(2,2)), 3);
        QCOMPARE(model.data(model.index(2,3)), "group 3");

        QCOMPARE(model.data(model.index(3,0)), 4);
        QCOMPARE(model.data(model.index(3,1)), "goods 4");
        QCOMPARE(model.data(model.index(3,2)), 1);
        QCOMPARE(model.data(model.index(3,3)), "group 1");

        model.setData(model.index(0,1), "supergoods 1");
        model.setData(model.index(0,2), 2);
        model.setData(model.index(2,1), "supergoods 3");
        model.setData(model.index(2,2), 2);

        bool submit = model.submit();
        qDebug()<<model.lastQuery();
        QCOMPARE(submit, true);
        QCOMPARE(model.select(), true);

        QCOMPARE(model.columnCount(), 4);
        QCOMPARE(model.rowCount(), 4);

        QCOMPARE(model.data(model.index(0,0)), 1);
        QCOMPARE(model.data(model.index(0,1)), "supergoods 1");
        QCOMPARE(model.data(model.index(0,2)), 2);
        QCOMPARE(model.data(model.index(0,3)), "group 2");

        QCOMPARE(model.data(model.index(1,0)), 2);
        QCOMPARE(model.data(model.index(1,1)), "goods 2");
        QCOMPARE(model.data(model.index(1,2)), 2);
        QCOMPARE(model.data(model.index(1,3)), "group 2");

        QCOMPARE(model.data(model.index(2,0)), 3);
        QCOMPARE(model.data(model.index(2,1)), "supergoods 3");
        QCOMPARE(model.data(model.index(2,2)), 2);
        QCOMPARE(model.data(model.index(2,3)), "group 2");

        QCOMPARE(model.data(model.index(3,0)), 4);
        QCOMPARE(model.data(model.index(3,1)), "goods 4");
        QCOMPARE(model.data(model.index(3,2)), 1);
        QCOMPARE(model.data(model.index(3,3)), "group 1");

     }


    void tst_04_insert()
    {
        SqlDatabase *sql = Datapipe::instance()->sql();
        QScopedPointer<SqlQuery> qry(sql->query());

        qry->remove("from goods");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (1, 'goods 1', 1)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (2, 'goods 2', 2)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (3, 'goods 3', 3)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (4, 'goods 4', 1)");

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

        //    {field:"goodsgroup_id", name:"Група", type:GuiTableModel.LookupColumn, foreign_table:"store.goodsgroups", foreign_key:"id", foreign_name:"name"},
        QCOMPARE(model.select(), true);
        // key and lookup columns will be added automatically
        QCOMPARE(model.rowCount(), 4);
        model.insertRows(model.rowCount(), 1);
        QCOMPARE(model.columnCount(), 4);
        QCOMPARE(model.rowCount(), 5);

        model.setData(model.index(4,1), "supergoods 1");
        model.setData(model.index(4,2), 2);

        qDebug()<<"(0,1)"<<model.data(model.index(0,1))
                <<"(1,1)"<<model.data(model.index(1,1))
                <<"(2,1)"<<model.data(model.index(2,1))
                <<"(3,1)"<<model.data(model.index(3,1));


        QCOMPARE(model.data(model.index(0,0)), 1);
        QCOMPARE(model.data(model.index(0,1)), "goods 1");
        QCOMPARE(model.data(model.index(0,2)), 1);
        QCOMPARE(model.data(model.index(0,3)), "group 1");

        QCOMPARE(model.data(model.index(1,0)), 2);
        QCOMPARE(model.data(model.index(1,1)), "goods 2");
        QCOMPARE(model.data(model.index(1,2)), 2);
        QCOMPARE(model.data(model.index(1,3)), "group 2");

        QCOMPARE(model.data(model.index(2,0)), 3);
        QCOMPARE(model.data(model.index(2,1)), "goods 3");
        QCOMPARE(model.data(model.index(2,2)), 3);
        QCOMPARE(model.data(model.index(2,3)), "group 3");

        QCOMPARE(model.data(model.index(3,0)), 4);
        QCOMPARE(model.data(model.index(3,1)), "goods 4");
        QCOMPARE(model.data(model.index(3,2)), 1);
        QCOMPARE(model.data(model.index(3,3)), "group 1");

        QCOMPARE(model.data(model.index(4,1)), "supergoods 1");
        QCOMPARE(model.data(model.index(4,2)), 2);

        model.revert();

        QCOMPARE(model.columnCount(), 4);
        QCOMPARE(model.rowCount(), 4);

        model.insertRows(model.rowCount(), 1);
        model.setData(model.index(4,1), "supergoods 1");
        model.setData(model.index(4,2), 2);

        QCOMPARE(model.submit(), true);
        QCOMPARE(model.select(), true);

        QCOMPARE(model.columnCount(), 4);
        QCOMPARE(model.rowCount(), 5);


        QCOMPARE(model.data(model.index(0,0)), 1);
        QCOMPARE(model.data(model.index(0,1)), "goods 1");
        QCOMPARE(model.data(model.index(0,2)), 1);
        QCOMPARE(model.data(model.index(0,3)), "group 1");

        QCOMPARE(model.data(model.index(1,0)), 2);
        QCOMPARE(model.data(model.index(1,1)), "goods 2");
        QCOMPARE(model.data(model.index(1,2)), 2);
        QCOMPARE(model.data(model.index(1,3)), "group 2");

        QCOMPARE(model.data(model.index(2,0)), 3);
        QCOMPARE(model.data(model.index(2,1)), "goods 3");
        QCOMPARE(model.data(model.index(2,2)), 3);
        QCOMPARE(model.data(model.index(2,3)), "group 3");

        QCOMPARE(model.data(model.index(3,0)), 4);
        QCOMPARE(model.data(model.index(3,1)), "goods 4");
        QCOMPARE(model.data(model.index(3,2)), 1);
        QCOMPARE(model.data(model.index(3,3)), "group 1");

        QCOMPARE(model.data(model.index(4,0)), 5);
        QCOMPARE(model.data(model.index(4,1)), "supergoods 1");
        QCOMPARE(model.data(model.index(4,2)), 2);
        QCOMPARE(model.data(model.index(4,3)), "group 2");

    }

    void tst_05_remove()
    {
        SqlDatabase *sql = Datapipe::instance()->sql();
        QScopedPointer<SqlQuery> qry(sql->query());

        qry->remove("from goods");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (1, 'goods 1', 1)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (2, 'goods 2', 2)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (3, 'goods 3', 3)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (4, 'goods 4', 1)");

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

        //    {field:"goodsgroup_id", name:"Група", type:GuiTableModel.LookupColumn, foreign_table:"store.goodsgroups", foreign_key:"id", foreign_name:"name"},
        QCOMPARE(model.select(), true);

        model.removeRows(model.rowCount()-1, 1);
        model.removeRows(1, 1);

        QCOMPARE(model.rowCount(), 2);

        QCOMPARE(model.data(model.index(0,0)), 1);
        QCOMPARE(model.data(model.index(0,1)), "goods 1");
        QCOMPARE(model.data(model.index(0,2)), 1);
        QCOMPARE(model.data(model.index(0,3)), "group 1");

        QCOMPARE(model.data(model.index(1,0)), 3);
        QCOMPARE(model.data(model.index(1,1)), "goods 3");
        QCOMPARE(model.data(model.index(1,2)), 3);
        QCOMPARE(model.data(model.index(1,3)), "group 3");

        model.revert();

        QCOMPARE(model.columnCount(), 4);
        QCOMPARE(model.rowCount(), 4);

        QCOMPARE(model.data(model.index(0,0)), 1);
        QCOMPARE(model.data(model.index(0,1)), "goods 1");
        QCOMPARE(model.data(model.index(0,2)), 1);
        QCOMPARE(model.data(model.index(0,3)), "group 1");

        QCOMPARE(model.data(model.index(1,0)), 2);
        QCOMPARE(model.data(model.index(1,1)), "goods 2");
        QCOMPARE(model.data(model.index(1,2)), 2);
        QCOMPARE(model.data(model.index(1,3)), "group 2");

        QCOMPARE(model.data(model.index(2,0)), 3);
        QCOMPARE(model.data(model.index(2,1)), "goods 3");
        QCOMPARE(model.data(model.index(2,2)), 3);
        QCOMPARE(model.data(model.index(2,3)), "group 3");

        QCOMPARE(model.data(model.index(3,0)), 4);
        QCOMPARE(model.data(model.index(3,1)), "goods 4");
        QCOMPARE(model.data(model.index(3,2)), 1);
        QCOMPARE(model.data(model.index(3,3)), "group 1");

        model.removeRows(model.rowCount()-1, 1);
        model.removeRows(1, 1);

        QCOMPARE(model.submit(), true);
        QCOMPARE(model.select(), true);

        QCOMPARE(model.rowCount(), 2);

        QCOMPARE(model.data(model.index(0,0)), 1);
        QCOMPARE(model.data(model.index(0,1)), "goods 1");
        QCOMPARE(model.data(model.index(0,2)), 1);
        QCOMPARE(model.data(model.index(0,3)), "group 1");

        QCOMPARE(model.data(model.index(1,0)), 3);
        QCOMPARE(model.data(model.index(1,1)), "goods 3");
        QCOMPARE(model.data(model.index(1,2)), 3);
        QCOMPARE(model.data(model.index(1,3)), "group 3");

    }


    void tst_06_mixed()
    {
        SqlDatabase *sql = Datapipe::instance()->sql();
        QScopedPointer<SqlQuery> qry(sql->query());

        qry->remove("from goods");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (1, 'goods 1', 1)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (2, 'goods 2', 2)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (3, 'goods 3', 3)");
        qry->insert("insert into goods (id, name, goodsgroup_id) values (4, 'goods 4', 1)");

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
        //    {field:"goodsgroup_id", name:"Група", type:GuiTableModel.LookupColumn, foreign_table:"store.goodsgroups", foreign_key:"id", foreign_name:"name"},
        QCOMPARE(model.select(), true);
        // key and lookup columns will be added automatically
        QCOMPARE(model.rowCount(), 4);

        model.setData(model.index(1,1), "megagoods 2");

        model.insertRows(4, 1);
        model.setData(model.index(4,1), "supergoods 5");
        model.setData(model.index(4,2), 2);

        model.insertRows(5, 1);
        model.setData(model.index(5,1), "supergoods 6");
        model.setData(model.index(5,2), 2);

        model.removeRows(4, 1);
        model.removeRows(2, 1);
        model.removeRows(0, 1);

        QCOMPARE(model.rowCount(), 3);

        QCOMPARE(model.data(model.index(0,0)), 2);
        QCOMPARE(model.data(model.index(0,1)), "megagoods 2");
        QCOMPARE(model.data(model.index(0,2)), 2);

        QCOMPARE(model.data(model.index(1,0)), 4);
        QCOMPARE(model.data(model.index(1,1)), "goods 4");
        QCOMPARE(model.data(model.index(1,2)), 1);

        QCOMPARE(model.data(model.index(2,1)), "supergoods 6");
        QCOMPARE(model.data(model.index(2,2)), 2);

        QCOMPARE(model.submit(), true);
        QCOMPARE(model.select(), true);

        QCOMPARE(model.rowCount(), 3);

        QCOMPARE(model.data(model.index(0,0)), 2);
        QCOMPARE(model.data(model.index(0,1)), "megagoods 2");
        QCOMPARE(model.data(model.index(0,2)), 2);

        QCOMPARE(model.data(model.index(1,0)), 4);
        QCOMPARE(model.data(model.index(1,1)), "goods 4");
        QCOMPARE(model.data(model.index(1,2)), 1);

        QCOMPARE(model.data(model.index(2,0)), 5);
        QCOMPARE(model.data(model.index(2,1)), "supergoods 6");
        QCOMPARE(model.data(model.index(2,2)), 2);
    }


};

QTEST_MAIN(tst_asqltablemodel)
#include "tst_asqltablemodel.moc"
