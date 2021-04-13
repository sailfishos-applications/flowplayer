import QtQuick 2.1
import Sailfish.Silica 1.0

MouseArea {
    id: popup
    anchors.top: parent.top
    width: parent.width
    height: Math.max(message.paintedHeight, icon.height) + Theme.paddingLarge
    property alias title: message.text
    property alias timeout: hideTimer.interval
    property alias background: bg.color
    visible: opacity > 0
    opacity: 0.0

    Behavior on opacity {
        FadeAnimation {}
    }

    Rectangle {
        id: bg
        anchors.fill: parent
        color: Theme.rgba(Theme.secondaryHighlightColor, 0.9)
    }

    Timer {
        id: hideTimer
        triggeredOnStart: false
        repeat: false
        interval: 5000
        onTriggered: popup.hide()
    }

    function hide() {
        if (hideTimer.running)
            hideTimer.stop()
        popup.opacity = 0.0
    }

    function show() {
        popup.opacity = 1.0
        hideTimer.restart()
    }

    function displayMessage(text, ok) {
        popup.title = text
        if (ok) icon.source = "image://theme/icon-s-installed"
        else icon.source = "image://theme/icon-s-high-importance"
        show()
    }

    Image {
        id: icon
        height: Theme.itemSizeSmall /2
        width: height
        smooth: true
        anchors.left: parent.left
        anchors.leftMargin: Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter
    }

    Label {
        id: message
        anchors.verticalCenter: popup.verticalCenter
        //font.pixelSize: 32
        anchors.left: icon.right
        anchors.leftMargin: Theme.paddingMedium
        anchors.right: parent.right
        anchors.rightMargin: Theme.paddingMedium
        //horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
        wrapMode: Text.Wrap
    }

    onClicked: hide()
}
