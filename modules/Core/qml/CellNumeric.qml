import QtQuick
import QtQuick.Controls

import modules.Core

Cell {
    id: cellRoot

    property int digits

    Label {
        id: cellContent
        anchors.fill: parent
        anchors.margins: Theme.BASE_MARGINS * 2

        text : cellRoot.cellData.toFixed(cellRoot.digits)??"0".toFixed(cellRoot.digits)
        font.pixelSize: Theme.TABLE_FONTSIZE
        clip: true
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight

        visible: !cellRoot.editing
    }
}


