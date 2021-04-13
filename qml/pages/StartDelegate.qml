import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

MouseArea {
    width: parent.width
    height: Theme.itemSizeExtraLarge

    property alias title: titleLabel.text
    property alias count: countLabel.text
    property alias model: image.model
    property alias running: timer.running
    property alias current: image.currentIndex

    function startTimer() {
        timer.start()
        timer.running = true
    }

    function stopTimer() {
        timer.stop()
        timer.running = false
    }

    Rectangle {
        id: content
        width: parent.width
        height: parent.height
        color: pressed? Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity) : "transparent"
    }

    Label {
        id: countLabel
        anchors.verticalCenter: image.verticalCenter
        anchors.right: image.left
        anchors.rightMargin: Theme.paddingLarge
        horizontalAlignment: Text.AlignRight
        color: Theme.secondaryColor
        visible: text!=="0"
        font.pixelSize: Theme.fontSizeSmall
    }

    PathView {
        id: image
        anchors.left: parent.left
        anchors.leftMargin: height
        height: parent.height
        width: height
        snapMode: ListView.SnapOneItem
        preferredHighlightBegin: 0.5
        preferredHighlightEnd: 0.5
        pathItemCount: 3
        clip: true
        enabled: false
        path: Path {
            startX: -image.width
            startY: image.height / 2
            PathLine { x: image.width; y: image.height / 2;  }
            PathLine { x: image.width *2; y: image.height / 2; }
        }
        delegate: Image {
            width: parent.width
            height: width
            clip: true
            source: model.url
            fillMode: Image.PreserveAspectCrop
            smooth: true
        }
    }

    Timer {
        id: timer
        interval: 5000
        repeat: true
        running: false
        onTriggered: image.incrementCurrentIndex()
    }

    Label {
        id: titleLabel
        anchors.verticalCenter: image.verticalCenter
        anchors.left: image.right
        anchors.leftMargin: Theme.paddingLarge
        width: parent.width - image.width*2 - Theme.paddingLarge*2
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }

}
