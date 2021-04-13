import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Dialog {

    id: dialog

    allowedOrientations: appWindow.pagesOrientations

    property bool editingPreset: false
    property string prevname

    onAccepted: {
        if (editingPreset)
            utils.removePreset(prevname)
        myPlayer.savePreset(ren.text, !editingPreset)
    }

    canAccept: ren.text.length>0

    DialogHeader {
        id: header
        acceptText: qsTr("Done")
        cancelText: qsTr("Cancel")
        spacing: 0
    }

    SilicaFlickable {
        anchors.top: parent.top
        anchors.topMargin: header.height
        width: parent.width
        height: parent.height
        contentHeight: col1.height+Theme.paddingLarge

        Column {
            id: col1
            width: parent.width
            spacing: Theme.paddingMedium

            PageHeader {
                id: header2
                title: editingRadio? qsTr("Edit preset") : qsTr("Save preset")
                anchors.top: header.bottom
                width: parent.width
            }

            TextField {
                id: ren
                width: parent.width
                text: prevname
                placeholderText: qsTr("Preset name")
                label: qsTr("Preset name")
                focus: true
                EnterKey.enabled: ren.text.length>0
                EnterKey.onClicked: ren2.forceActiveFocus()
            }

        }

    }

}
