import QtQuick 1.1
import com.nokia.meego 1.0
import FlowPlayer 1.0

Item
{
    id: itemcontainer
    signal clicked
    signal pressAndHold
    signal doubleClicked

    property alias name: name.text
    property alias desc: desc.text
    property int cindex
    property string isplaying

    height: 62
    width: parent.width
    clip: true

    BorderImage
    {
        id: background
        anchors.fill: itemcontainer
        visible:  mouseArea.pressed ? true : false
        source: "image://theme/meegotouch-list-fullwidth" + (theme.inverted ? "-inverted" : "") + "-background-pressed-center"
    }

    Text
    {
        id: name
        //textFormat: Text.RichText
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 6
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        width: itemcontainer.width -20
        font.bold: currentTheme==="blanco"
        font.pixelSize: 22
        height: 24
        elide: Text.ElideRight
        color: isplaying == "true" ? themeColor : "white"
    }

    Text
    {
        id: desc
        //anchors.verticalCenter: parent.verticalCenter
        anchors.top: name.bottom
        anchors.left: parent.left
        elide: Text.ElideRight
        anchors.topMargin: 3
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        width: itemcontainer.width -20
        font.pixelSize: 18
        color: isplaying == "true" ? themeColor : (currentTheme==="blanco"? "#8c8e8c" : "white")
    }



    MouseArea
    {
        id: mouseArea
        anchors.fill: itemcontainer
        onClicked: itemcontainer.clicked()
        //onDoubleClicked: itemcontainer.doubleClicked()
        //onPressAndHold: itemcontainer.pressAndHold()
    }
}
