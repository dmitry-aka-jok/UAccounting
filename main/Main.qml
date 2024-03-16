import QtQuick
import QtQuick.Window

import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import Qt.labs.qmlmodels

import modules.Core

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("UAccounting")


    HorizontalHeaderView {
        id: horizontalHeader
        anchors.left: tableView.left
        anchors.top: parent.top
        syncView: tableView
        clip: true

        delegate:
            Rectangle {
            id: delegateRoot
            border.color: Theme.TABLE_COLOR_LINES
            border.width: 1
            implicitHeight: Theme.BASE_HEIGHT

            onWidthChanged: {
                table.storeColumnWidth(column, width)
            }

            Text {
                anchors.fill: parent
                anchors.rightMargin: sortHandle.width

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.TABLE_FONTSIZE
                font.bold: true

                wrapMode:TextEdit.WordWrap

                clip: true

                text: table.guiTableModel.columnHeaderName(column)

            }

            IconImage {
                id: sortHandle
                //visible: headerDelegate.active

                source: (table.sortField!==column||table.sortOrder===0?
                             "qrc:/modules/Core/icons/fi-bs-arrows-h-copy.svg"
                           :(table.sortOrder===1?"qrc:/modules/Core/icons/fi-bs-arrow-down.svg":"qrc:/modules/Core/icons/fi-bs-arrow-up.svg"))

                color: Theme.ICON_COLOR

                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 1
                anchors.topMargin: 1
                anchors.bottomMargin: 1

                width: 25
                height: parent.height
                fillMode: Image.PreserveAspectFit

            }

            MouseArea {
                anchors.fill: parent
                //enabled: headerDelegate.active
                onClicked: {
                    if (table.sortField === column){
                        table.sortOrder += 1
                        if(table.sortOrder > 2)
                            table.sortOrder = 0
                    }else{
                        table.sortField = column
                        table.sortOrder = 1
                    }
                }
            }

        }
    }




    TableView {
        id: tableView
        resizableColumns:true
        editTriggers: TableView.AnyKeyPressed | TableView.SingleTapped

        anchors.fill: parent
        anchors.topMargin: horizontalHeader.height


        delegate: DelegateChooser {
            role:"Type"

            DelegateChoice {
                roleValue: GuiTableModel.StringColumn
                CellString {
                    id: cellString
                    implicitWidth: table.columnWidth(column, "string")
                    implicitHeight: Theme.BASE_HEIGHT
                    cellData: Data

                    TableView.editDelegate:
                        TextField {
                        x: cellString.x
                        y: cellString.y
                        width: cellString.width
                        height: cellString.height
                        text: cellString.cellData
                        TableView.onCommit: Data = text
                    }
                }
            }

            DelegateChoice {
                roleValue: GuiTableModel.NumericColumn
                CellNumeric {
                    implicitWidth: table.columnWidth(column, "numeric")
                    implicitHeight: Theme.BASE_HEIGHT
                    cellData: Data
                    digits: Digits
                }
            }

            DelegateChoice {
                roleValue: GuiTableModel.BoolColumn
                CellBool {
                    implicitWidth: table.columnWidth(column, "bool")
                    implicitHeight: Theme.BASE_HEIGHT
                    cellData: Data
                }
            }

            DelegateChoice {
                roleValue: GuiTableModel.LookupColumn
                CellString {
                    implicitWidth: table.columnWidth(column, "string")
                    implicitHeight: Theme.BASE_HEIGHT
                    cellData: Data
                }
            }
        }

    }

    GuiTable {
        id: table

        table:"goods"
        fields : [
            {field:"name", name:"Назва", type:GuiTableModel.StringColumn},
            {field:"group_id", name:"Група", type:GuiTableModel.LookupColumn, foreign_table:"goodsgroups", foreign_key:"id", foreign_name:"name"},
            {field:"price_buy", name:"Ціна придбання", type:GuiTableModel.NumericColumn, digits:2},
            {field:"price_sell", name:"Ціна продажу", type:GuiTableModel.NumericColumn, digits:2},
        ];

        Component.onCompleted: {
            execStack = new Error().stack;
            init();
            tableView.model = guiTableModel;
        }
    }






    Component.onCompleted: {
        Datapipe.setVariable("debug_level", 2);

        Theme.theme = Material.theme
        Theme.BASE_HEIGHT = Material.buttonHeight * 1.5
        Theme.TABLE_COLOR_LINES = Material.primaryColor
        Theme.TABLE_COLOR_EVEN  = Material.listHighlightColor
        Theme.TABLE_COLOR_ODD   = "transparent"
    }

}
