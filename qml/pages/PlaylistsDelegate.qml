import QtQuick 1.1
import com.nokia.meego 1.0
import FlowPlayer 1.0
import "dateandtime.js" as DT


Item
{
        id: itemcontainer
        property variant myData

        signal clicked
        signal pressAndHold
        signal doubleClicked

        property string name
        property string desc
        property string time
        property string img
        property string count
        property int cindex
        property string isplaying

        //height: 78
        width: parent.width
        clip: true

        BorderImage
        {
            id: background
            anchors.fill: itemcontainer
            visible:  mouseArea.pressed ? true : false
            source: "image://theme/meegotouch-list-fullwidth" + (theme.inverted ? "-inverted" : "") + "-background-pressed-center"
        }

        Image {
            height: 62
            width: img!="" ? 62 : 0
            anchors.top: parent.top
            anchors.topMargin: 8
            anchors.left: parent.left
            anchors.leftMargin: 10
            source: getCover("qrc:/images/nocover62.jpg")
        }

        Image {
            id: coverImg
            height: 62
            width: img!="" ? 62 : 0
            anchors.top: parent.top
            anchors.topMargin: 8
            anchors.left: parent.left
            anchors.leftMargin: 10
            source: img
            states: [
                State {
                    name: 'loaded'; when: coverImg.status==Image.Ready
                    PropertyChanges { target: coverImg; scale: 1; opacity: 1; }
                },
                State {
                    name: 'loading'; when: coverImg.status!=Image.Ready
                    PropertyChanges { target: coverImg; scale: 1; opacity: 0; }
                }
            ]
            transitions: Transition {
                NumberAnimation { properties: "scale, opacity"; easing.type: Easing.InOutQuad; duration: 1000 }
            }

        }


        Text
        {
            text: name
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: img!="" ? 12 : 8
            anchors.leftMargin: img!="" ? 84 : 10
            anchors.rightMargin: 10
            width: img!="" ? itemcontainer.width -84 : itemcontainer.width -20
            font.weight: Font.Bold
            font.pixelSize: 22
            elide: Text.ElideRight
            color: isplaying == "true" ? themeColor : "white"
        }

        Text
        {
            text: desc
            anchors.top: parent.top
            anchors.left: parent.left
            elide: Text.ElideRight
            anchors.topMargin: img!="" ? 44 : 40
            anchors.leftMargin: img!="" ? 84 : 10
            anchors.rightMargin: 10
            width: img=="yes" ? itemcontainer.width -114 : itemcontainer.width -40
            font.pixelSize: 18
            color: isplaying == "true" ? themeColor : (currentTheme==="blanco"? "#8c8e8c" : "white")
        }

        Text
        {
            text: time
            anchors.top: parent.top
            anchors.topMargin: img!="" ? 44 : 40
            horizontalAlignment: Text.AlignRight
            width: itemcontainer.width -10
            font.pixelSize: 18
            color: isplaying == "true" ? themeColor : (currentTheme==="blanco"? "#8c8e8c" : "white")
        }


        MouseArea
        {
            id: mouseArea
            anchors.fill: itemcontainer
            onClicked: itemcontainer.clicked()
            //onDoubleClicked: itemcontainer.doubleClicked()
            onPressAndHold: itemcontainer.pressAndHold()
        }
}
