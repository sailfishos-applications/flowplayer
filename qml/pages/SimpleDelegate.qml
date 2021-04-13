import QtQuick 2.0
import Sailfish.Silica 1.0

ListItem {
    property alias iconimage: icon.source
    property alias title: label.text

    contentHeight: Theme.itemSizeMedium
    width: parent.width
    clip: true

    Image {
        id: icon
        anchors.left: parent.left
        anchors.leftMargin: Theme.paddingLarge
        anchors.verticalCenter: parent.verticalCenter
        source: model.icon
    }

    Label {
        id: label
        anchors.left: icon.right
        anchors.leftMargin: Theme.paddingMedium
        anchors.right: parent.right
        anchors.rightMargin: Theme.paddingLarge
        anchors.verticalCenter: parent.verticalCenter
        text: model.name
        truncationMode: TruncationMode.Fade
    }
}
