import QtQuick
import QtQuick.Controls

import modules.Core

Cell {
    id: cellRoot

    Label {
        id: cellContent
        anchors.fill: parent
        anchors.margins: Theme.BASE_MARGINS * 2
        verticalAlignment: Text.AlignVCenter

        text : cellRoot.cellData
        font.pixelSize: Theme.TABLE_FONTSIZE
        clip: true

        visible: !cellRoot.editing

    }

}
