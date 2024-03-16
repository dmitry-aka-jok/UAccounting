import QtQml
import QtQml.Models

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Qt.labs.qmlmodels

import modules.Core

GuiTable {
    id: tableRoot
    anchors.fill: parent

    Component.onCompleted: {
        // console.log(editor)
        // const row = 0;
        // const isNew = true;
        // const editObj = editor.createObject(mainView,
        //                                         {sqlRow: guiTableModel.sourceRow(row) ,
        //                                         sqlModel: sqlTableModel,
        //                                         isNew: isNew,
        //                                         taskName: isNew?invitationAdd:invitationEdit});
        execStack = new Error().stack;

        init();

        tableView.model = guiTableModel;
    }


}

