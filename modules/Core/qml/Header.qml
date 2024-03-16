import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

import modules.Core

Item {
    id: headerRoot
    implicitHeight: Theme.BASE_HEIGHT

    property alias text: label.text

    Label {
        id: label

        anchors.fill: parent
        anchors.rightMargin: sortHandle.width

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.bold: true

        clip: true
    }


    property bool active: true

    signal sortClicked()

    Rectangle {
        anchors.fill : parent
        border.color: Theme.TABLE_COLOR_LINES
        border.width: 1
        color: Theme.TABLE_COLOR_ODD
    }


    IconImage {
      id: sortHandle
      visible: headerDelegate.active

       source: (tableRoot.sortField!==column||tableRoot.sortOrder===0?
                   "qrc:/modules/Core/icons/fi-bs-arrows-h-copy.svg"
                 :(tableRoot.sortOrder===1?"qrc:/modules/Core/icons/fi-bs-arrow-down.svg":"qrc:/modules/Core/icons/fi-bs-arrow-up.svg"))

      color: Theme.ICON_COLOR

      anchors.verticalCenter: parent.verticalCenter
      anchors.right: parent.right
      anchors.rightMargin: 1
      anchors.topMargin: 1
      anchors.bottomMargin: 1

      width: 15
      height: headerRoot.height
      fillMode: Image.PreserveAspectFit

    }

    MouseArea {
      anchors.fill: parent
      enabled: headerDelegate.active
      onClicked: {
        headerRoot.sortClicked()
      }
    }

}
