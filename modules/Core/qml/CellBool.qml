import QtQuick
import QtQuick.Controls

Cell {
    id: cellRoot

    Switch {
        anchors.centerIn: parent
        checked: cellRoot.cellData
        checkable: false
    }
}
