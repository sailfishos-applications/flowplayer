import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0
import org.nemomobile.thumbnailer 1.0


BackgroundItem
{
    id: itemcontainer
    //signal clicked
    //signal pressAndHold
    //signal doubleClicked

    property int itemheight: coverImg.paintedHeight
    property int itemwidth: coverImg.paintedWidth
    property string itemimg
    property alias text: vtext.text
    property bool textvisible: false

    property alias fillMode: coverImg.fillMode
    property alias status: coverImg.status

    property int cindex

    property bool showBorder: false
    property alias textSize: vtext.font.pixelSize

    property string artist
    property string album

    height: itemheight
    width: itemwidth
    clip: true

    onItemimgChanged: reload()

    function reload() {
        coverImg.source = ""
        coverImg.source = itemimg
        coverImg.sourceSize.width = 0
        coverImg.sourceSize.height = 0
        coverImg.sourceSize.width = lastGroup=="artists"? coverImg.width*1.5 : coverImg.width
        coverImg.sourceSize.height = lastGroup=="artists"? coverImg.height*1.5 : coverImg.height
    }

    Connections {
        target: missing
        onAlbumCoverChanged: {
            if (itemimg===coverpath) {
                console.log("Saved: " + itemimg + " - " + coverpath)
                reload()
            }
        }
    }

    Connections {
        target: lfm
        onBioImageChanged: {
            if (itemimg===imagepath) {
                console.log("Saved: " + artist + ":" + album + " -- " + itemimg + " - " + imagepath)
                reload()
            }
        }
    }

    Connections {
        target: coversearch
        onImageChanged: {
            if ((artist===dartist && album==dalbum)
                || (album===dartist && album==dalbum)
                || (album===dartist && !dalbum)) {
                console.log("Updated: " + artist + " - " + album)
                reload()
            }
        }
    }

    Rectangle
    {
        id: mainCover
        anchors.fill: parent
        anchors.margins: showBorder? 0 : 1
        color: Qt.darker(Theme.highlightColor, 2.5)
        opacity: showBorder? 1 : 0.5
        border.color: Theme.highlightColor
        border.width: showBorder? 1 : 0
    }

    Image {
        id: coverImg
        source: itemimg=="loading" ? "" : itemimg
        anchors.fill: parent
        sourceSize.width: lastGroup=="artists"? width *1.25 : width
        sourceSize.height: lastGroup=="artists"? height *1.25 : height
        fillMode: Image.PreserveAspectCrop
        smooth: true
        asynchronous: true
        cache: false
    }


    Rectangle {
        id: shadow
        visible: coverImg.status!=Image.Error && textvisible
        height: vtext.paintedHeight + Theme.paddingLarge*2
        anchors.bottom: parent.bottom
        width: parent.width

        gradient: Gradient {
            GradientStop { position: 0; color: "transparent" }
            GradientStop { position: 1; color: "black" }
        }

    }

    Label {
        id: vtext
        y: coverImg.status!=Image.Error && textvisible
            ? parent.height - height - Theme.paddingSmall
            : (parent.height - height) / 2
        anchors.left: parent.left
        anchors.leftMargin: Theme.paddingMedium
        width: parent.width - Theme.paddingMedium*2
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeExtraSmall*0.9
        //text: "<b>" + album + "</b>" + (artist!==""? "<br>" : "") + artist
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        visible: coverImg.status==Image.Error || itemimg==="" || textvisible
    }

}
