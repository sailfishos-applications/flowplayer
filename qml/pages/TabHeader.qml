import QtQuick 2.1
import Sailfish.Silica 1.0

Item {
    id: tabHeader

    property SlideshowView listView: null
    property variant textArray: []
    property int visibleHeight: flickable.contentY + height
    property real labelSize: Theme.fontSizeLarge
    //signal tabClicked(int number)

    SilicaFlickable {
        id: flickable
        anchors.fill: parent
        contentHeight: parent.height

        Row {
            anchors.fill: parent

            Repeater {
                id: sectionRepeater
                model: textArray
                delegate: BackgroundItem {

                    width: tabHeader.width / sectionRepeater.count
                    height: tabHeader.height

                    Label {
                        id: labelText
                        anchors.centerIn: parent
                        color: Theme.highlightColor
                        font.pixelSize: labelSize
                        text: modelData
                    }

                    onClicked: {
                        if (listView.currentIndex!==index) {
                            if (listView.count>2)
                                listView.currentIndex = index
                            else if (listView.currentIndex<index)
                                listView.incrementCurrentIndex()
                            else
                                listView.decrementCurrentIndex()

                        }
                    }
                }
            }
        }

        Rectangle {
            id: currentSectionIndicator
            anchors.bottom: parent.bottom
            color: Theme.highlightColor
            height: Theme.paddingSmall
            width: tabHeader.width / sectionRepeater.count
            x: listView.currentIndex * width

            Behavior on x {
                NumberAnimation {
                    duration: 200
                }
            }
        }

    }
}
