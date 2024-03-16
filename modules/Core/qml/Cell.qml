import QtQuick
import QtQuick.Controls

import modules.Core

Item {
    property variant cellData

    required property bool current // - true if the delegate is current.
    required property bool selected // - true if the delegate is selected.
    required property bool editing // - true if the delegate is being edited.

/*
    Rectangle {
        anchors.top: parent.top
        visible: row === 0 || selected
        color: Theme.TABLE_COLOR_LINES
        height: selected ? 2 : 1
        width: parent.width
    }
    */
    Rectangle {
        anchors.fill : parent
        border.color: Theme.TABLE_COLOR_LINES
        border.width: 1
        color: (row % 2 == 0) ? Theme.TABLE_COLOR_EVEN : Theme.TABLE_COLOR_ODD
    }

/*
    Rectangle {
        anchors.top: parent.top
        visible: row === 0 || selectedRow===row
        color: (selectedRow===row) ? Material.accentColor : Material.dividerColor
        height: ((selectedColumn===column || selectedColumn===-1) && selectedRow===row) ? 2 : 1
        width: parent.width
    }
    Rectangle {
        anchors.bottom: parent.bottom
        color: (selectedRow===row) ? Material.accentColor : Material.dividerColor
        height: ((selectedColumn===column || selectedColumn===-1) && selectedRow===row) ? 2 : 1
        width: parent.width
    }

    Rectangle {
        anchors.left: parent.left
        visible: column === 0
        color: Material.dividerColor
        height: parent.height
        width: 1
    }
    Rectangle {
        anchors.right: parent.right
        color: Material.dividerColor
        height: parent.height
        width: 1
    }

    Rectangle {
        anchors.left: parent.left
        anchors.top: parent.top
        visible:  (selectedColumn===column && selectedRow===row)
        color: Material.accentColor
        width: 2
        height: parent.height / 6
    }
    Rectangle {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        visible:  (selectedColumn===column && selectedRow===row)
        color: Material.accentColor
        width: 2
        height: parent.height / 6
    }
    Rectangle {
        anchors.right: parent.right
        anchors.top: parent.top
        visible:  (selectedColumn===column && selectedRow===row)
        color: Material.accentColor
        width: 2
        height: parent.height / 6
    }
    Rectangle {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        visible:  (selectedColumn===column && selectedRow===row)
        color: Material.accentColor
        width: 2
        height: parent.height / 6
    }

    //highlighted: row % 2 == 0
    Rectangle {
        anchors.fill : parent
        color: (row % 2 == 0) ? Material.listHighlightColor : "transparent"
    }


    Loader {
        id: editorLoader
        anchors.fill: parent
        anchors.leftMargin: Theme.BASE_MARGINS * 2

        Connections {
            target: editorLoader.item
            function onCommited(){
                cellRoot.state = "";
                editorLoader.sourceComponent = undefined
            }
            function onRejected(){
                cellRoot.state = "";
                editorLoader.sourceComponent = undefined
            }
        }

        Binding {target: editorLoader.item; property: "sqlModel";   value: delegateRoot.sqlModel}
        Binding {target: editorLoader.item; property: "sqlField";   value: delegateRoot.sqlField}
        Binding {target: editorLoader.item; property: "sqlRow";     value: delegateRoot.sqlRow}
        Binding {target: editorLoader.item; property: "autosave";   value: delegateRoot.autosave}
    }



    function editSourceComponent(editor){
        cellRoot.state = "edit"
        editorLoader.sourceComponent = editor
        editorLoader.item.startEdit()
    }
    function editSource(editor){
        cellRoot.state = "edit"
        editorLoader.source = editor
        editorLoader.item.startEdit()
    }

    states: [
        State {name: ""},
        State {name: "edit"}
    ]
*/
}
