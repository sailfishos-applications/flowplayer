import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Item
{
    id: itemcontainer
    property variant myData

    signal clicked

    property string name
    property string desc
    property string img
    property bool isSel
    property bool loadSel
    property int cindex

    signal loadFinished()

    width: parent.width
    clip: true

    CoverArtList {
        id: thumb
        x: Theme.paddingLarge
        width: Theme.itemSizeMedium - Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter
        height: width
        itemimg: img==="" || img==="ERROR"? "" : img
        text: loadSel && img!=="ERROR"? "" : img==="ERROR"? qsTr("Not found") : qsTr("No cover")
    }

    /*CoverArtList {
        id: coverImg
        x: Theme.paddingLarge
        anchors.verticalCenter: parent.verticalCenter
        width: Theme.itemSizeMedium - Theme.paddingMedium
        height: width
        itemimg: img
    }*/

    BusyIndicator {
        id: busy
        size: BusyIndicatorSize.Small
        anchors.centerIn: thumb
        visible: loadSel && img===""
        running: visible
        onVisibleChanged: { if (img!="") itemcontainer.loadFinished() }
    }

    /*Image {
        id: done
        height: 28
        width: 28
        smooth: true
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.verticalCenter: parent.verticalCenter
        source: img=="ERROR"? "qrc:/images/search-error.png" : "qrc:/images/search-ok.png"
        visible: img!="" && !busy.visible
    }*/

    Column {
        anchors.left: thumb.right
        anchors.leftMargin: Theme.paddingMedium
        anchors.right: parent.right
        anchors.rightMargin: Theme.paddingLarge
        anchors.verticalCenter: parent.verticalCenter

        Label
        {
            text: name
            font.pixelSize: Theme.fontSizeMedium
            color: Theme.primaryColor
            truncationMode: TruncationMode.Fade
            width: parent.width
        }

        Label
        {
            text: desc
            truncationMode: TruncationMode.Fade
            width: parent.width
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryColor
        }

    }

}
