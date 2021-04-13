import QtQuick 1.0
import com.nokia.meego 1.0

MyMenuItem {
    id: root

    property alias title: title.text
    property string initialValue
    property alias model: selectionDialog.model
    property alias pressed: mouseArea.pressed

    signal valueChosen(string value)
    signal clicked

    Item {
        id: selector

        anchors.fill: parent

        Component.onCompleted: {
            var found = false;
            var i = 0;
            while ((!found) && (i < model.count)) {
                if (model.get(i).value == initialValue) {
                    selectionDialog.selectedIndex = i;
                    found = true;
                }
                i++;
            }
        }

        Column {

            anchors { left: parent.left; leftMargin: 16; verticalCenter: parent.verticalCenter }

            Label {
                id: title
                font.pointSize: 18
                font.bold: true
                color: "white"
                verticalAlignment: Text.AlignVCenter
            }

            Label {
                id: subTitle
                font.pointSize: 16
                color: "#8d8d8d"
                verticalAlignment: Text.AlignVCenter
                text: qsTr(model.get(selectionDialog.selectedIndex).name)
            }
        }

        Image {
            anchors { right: parent.right; rightMargin: 20; verticalCenter: parent.verticalCenter }
            source: "file:///usr/share/themes/blanco/meegotouch/icons/icon-m-textinput-combobox-arrow.png"
            sourceSize.width: width
            sourceSize.height: height
        }

        SelectionDialog {
            id: selectionDialog
            titleText: title.text
            onAccepted: valueChosen(model.get(selectedIndex).value)
            delegate: Item {
                id: delegateItem
                property bool selected: index == selectedIndex;

                height: root.platformStyle.itemHeight
                anchors.left: parent.left
                anchors.right: parent.right

                MouseArea {
                    id: delegateMouseArea
                    anchors.fill: parent;
                    onPressed: selectedIndex = index;
                    onClicked:  accept();
                }


                Rectangle {
                    id: backgroundRect
                    anchors.fill: parent
                    color: delegateItem.selected ? root.platformStyle.itemSelectedBackgroundColor : root.platformStyle.itemBackgroundColor
                }

                BorderImage {
                    id: background
                    anchors.fill: parent
                    //border { left: UI.CORNER_MARGINS; top: UI.CORNER_MARGINS; right: UI.CORNER_MARGINS; bottom: UI.CORNER_MARGINS }
                    source: delegateMouseArea.pressed ? root.platformStyle.itemPressedBackground : ""
                }

                Text {
                    id: itemText
                    elide: Text.ElideRight
                    color: delegateItem.selected ? root.platformStyle.itemSelectedTextColor : root.platformStyle.itemTextColor
                    anchors.verticalCenter: delegateItem.verticalCenter
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: root.platformStyle.itemLeftMargin
                    anchors.rightMargin: root.platformStyle.itemRightMargin
                    text: qsTr(name)
                    font: root.platformStyle.itemFont
                }
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            selectionDialog.open();
            if (parent.enabled) {
                parent.clicked();
            }
        }
    }

    //onClicked: if (parent) parent.closeLayout();
}
