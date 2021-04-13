import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Dialog {

    id: dialog

    allowedOrientations: appWindow.pagesOrientations

    property bool editingRadio: false
    property string prevname
    property string prevurl
    property string previd: ""
    property string previmage: ""

    onAccepted: {
        if (editingRadio)
            radios.removeRadio(prevname, prevurl)
        radios.saveRadio(ren.text, ren2.text, previd, previmage)
        radioModel.clear()
        radios.loadRadios()
    }

    canAccept: ren.text.length>0 && ren2.text.length>0

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
        //color: "transparent"

        Column {
            id: col1
            width: parent.width
            spacing: Theme.paddingMedium

            PageHeader {
                id: header2
                title: editingRadio? qsTr("Edit station") : qsTr("Add station")
                anchors.top: header.bottom
                width: parent.width
            }

            TextField {
                id: ren
                width: parent.width
                text: prevname
                placeholderText: qsTr("Station name")
                label: qsTr("Station name")
                focus: true
                EnterKey.enabled: ren.text.length>0
                EnterKey.onClicked: ren2.forceActiveFocus()
            }

            TextField {
                id: ren2
                width: parent.width
                text: prevurl
                placeholderText: qsTr("Station url")
                label: qsTr("Station url")
                focus: true
                EnterKey.enabled: ren2.text.length>0
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: dialog.accept()
            }

        }

    }

}
