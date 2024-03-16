import QtQuick
import QtQuick.Controls

import QtTest

import modules.Core
//import "../../modules/Core"

Item {

    SqlDatabase {
        id: db

        driver: 'QSQLITE'
        database: ':memory:'

        Component.onCompleted: {

            const qry = db.query()

            qry.execute("CREATE TABLE [goodsgroups] (\
                     [id] INTEGER PRIMARY KEY,\
                     [name] TEXT,\
                     [remark] TEXT\
                     );");

            qry.execute("CREATE TABLE [goods] (\
                     [id] INTEGER PRIMARY KEY,\
                     [name] TEXT,\
                     [goodsgroup_id] INTEGER,\
                     [is_weight] INTEGER,\
                     [price_buy] FLOAT,\
                     [price_sell] FLOAT,\
                     [remark] TEXT,\
                     FOREIGN KEY([goodsgroup_id]) REFERENCES [goodsgroups]([id])\
                     );");

            qry.setStructure({
                                foreign_keys: [
                                    {
                                        field: "goodsgroup_id",
                                        foreign_key: "id",
                                        foreign_table: "goodsgroups",
                                        table: "goods"
                                    },
                                    {
                                        field: "goods_id",
                                        foreign_key: "id",
                                        foreign_table: "goods",
                                        table: "goods_barcodes"
                                    }
                                ],
                                primary_keys: [
                                    {
                                        field: "id",
                                        table: "goods"
                                    },
                                    {
                                        field: "id",
                                        table: "goods_barcodes"
                                    },
                                    {
                                        field: "id",
                                        table: "goodsgroups"
                                    }
                                ]
                            });
        }

    }

    TestCase {
        name: "tst_sql"

        function initTestCase() {

        }

        function cleanupTestCase() {
        }

        function test_01_sql() {
            Datapipe.setVariable("debug_level", 2);
            const qry = db.query()

            qry.insert({into:"goodsgroups", values:{id:1, name:"group 1"}});
            compare(qry.lastQuery(), "INSERT INTO goodsgroups (id,name) VALUES (1,'group 1')");
            qry.insert({into:"goodsgroups", values:{id:2, name:"group 2"}});
            qry.insert({into:"goodsgroups", values:{id:3, name:"group 3"}});

            qry.insert({into:"goods", values:{id:1, name:"goods 1", goodsgroup_id:1}});
            qry.insert({into:"goods", values:{id:2, name:"goods 2", goodsgroup_id:1}});
            qry.insert({into:"goods", values:{id:3, name:"goods 3", goodsgroup_id:1}});
            qry.insert({into:"goods", values:{id:4, name:"goods 4", goodsgroup_id:2}});
            qry.insert({into:"goods", values:{id:5, name:"goods 5", goodsgroup_id:2}});
            qry.insert({into:"goods", values:{id:6, name:"goods 6", goodsgroup_id:3}});

            const sel1 = qry.select("select id, name from goodsgroups where id = 1");
            compare(qry.lastQuery(), "select id, name from goodsgroups where id = 1");
            //compare(sel1, true);

            compare(qry.next(), true);
            compare(qry.value(0), 1);
            compare(qry.value(1), "group 1");
            compare(qry.next(), false);

            const sel2 = qry.select({from:"goodsgroups", fields:["id", "name"],  where:{id:1}});
            compare(qry.lastQuery(), "SELECT id, name FROM goodsgroups WHERE (id=1)");
            //compare(sel2, true);

            compare(qry.next(), true);
            compare(qry.value(0), 1);
            compare(qry.value(1), "group 1");
            compare(qry.next(), false);

            qry.select({from:"goodsgroups gg", fields:["id", "name", "sum(id)"], group: ["id", "name"], order:["id","name"]});
            compare(qry.lastQuery(), "SELECT id, name, sum(id) FROM goodsgroups gg GROUP BY id,name ORDER BY id, name");

            compare(qry.next(), true);
            compare(qry.value(0), 1);
            compare(qry.value(1), "group 1");
            compare(qry.next(), true);
            compare(qry.value(0), 2);
            compare(qry.value(1), "group 2");
            compare(qry.next(), true);
            compare(qry.value(0), 3);
            compare(qry.value(1), "group 3");

            qry.select({from:"goods g", join:"goodsgroups gg", fields:["g.id", "g.name"], where:"gg.id=1"});

            compare(qry.next(), true);
            compare(qry.value(0), 1);
            compare(qry.value(1), "goods 1");
            compare(qry.next(), true);
            compare(qry.value(0), 2);
            compare(qry.value(1), "goods 2");
            compare(qry.next(), true);
            compare(qry.value(0), 3);
            compare(qry.value(1), "goods 3");

            qry.update({table:"goodsgroups", set:{name:"group 222"}, where:{id:2}})
            compare(qry.lastQuery(), "UPDATE goodsgroups SET name='group 222' WHERE (id=2)");

            qry.select({from:"goodsgroups", fields:"name",  where:"id=2"});
            compare(qry.value("name"), "group 222");

            qry.remove({from:"goodsgroups", where:"id=2"});
            compare(qry.lastQuery(), "DELETE FROM goodsgroups WHERE id=2");

            qry.select({from:"goodsgroups", fields:"count(*)"});
            compare(qry.value(0), 2);

            qry.replace({into:"goods", values:{id:7, name:"goods 7", goodsgroup_id:3}});
            qry.select({from:"goods", fields:"name",  where:{id:7}});
            compare(qry.value("name"), "goods 7");

            qry.replace({into:"goods", values:{id:7, name:"goods 8", goodsgroup_id:2}});
            qry.select({from:"goods", fields:"name",  where:{id:7}});
            compare(qry.value("name"), "goods 8");
        }
    }
}
