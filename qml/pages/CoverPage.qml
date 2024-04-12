import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

CoverBackground {

    Image {
        visible: !playerOpened
        source: "file:///usr/share/icons/hicolor/86x86/apps/flowplayer.png"
        anchors.top: parent.top
        anchors.topMargin: Theme.paddingLarge*2
        anchors.horizontalCenter: parent.horizontalCenter
        opacity: 0.6
    }

    Label {
        visible: !playerOpened
        anchors.centerIn: parent
        text: "FlowPlayer"
        font.pixelSize: Theme.fontSizeSmall
        color: Theme.secondaryColor
        opacity: 0.8
    }

    CoverArtList {
        id: thumb
        visible: playerOpened
        width: showBigCover? height : parent.width
        height: showBigCover? parent.height : width
        anchors.horizontalCenter: parent.horizontalCenter
        //y: Theme.paddingMedium
        //x: Theme.paddingMedium
        itemimg: playingRadio? currentSongInfo.imageurl : utils.thumbnail(currentSongInfo.artist, currentSongInfo.album)
        textSize: Theme.fontSizeSmall
        text: playingRadio? currentSongInfo.name : qsTr("Cover not found")
        opacity: showBigCover? 0.4 : 1
    }

    Label {
        visible: playerOpened && showBigCover
        x: Theme.paddingMedium
        width: parent.width - Theme.paddingMedium*2
        text: currentSongInfo!==[]? currentSongInfo.title : ""
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.paddingLarge*3
        font.pixelSize: Theme.fontSizeMedium
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        horizontalAlignment: Text.AlignHCenter
        color: Theme.primaryColor
    }

    Label {
        visible: playerOpened && !showBigCover
        x: Theme.paddingMedium
        width: parent.width - Theme.paddingMedium*2
        text: currentSongInfo!==[]? currentSongInfo.title : ""
        anchors.top: thumb.bottom
        anchors.topMargin: Theme.paddingLarge
        font.pixelSize: Theme.fontSizeExtraSmall
        truncationMode: TruncationMode.Fade
        color: Theme.secondaryColor
    }

    CoverActionList {
        id: coverActionPrev
        enabled: playerOpened && showPrevCover

        CoverAction {
            iconSource: "image://theme/icon-cover-previous-song"
            onTriggered: {
                nowPlayingPage.prevSong()
            }
        }

        CoverAction {
            //enabled: player.source!==""
            iconSource: myPlayer.state===2? "image://theme/icon-cover-play" : "image://theme/icon-cover-pause"
            onTriggered: {
                if (myPlayer.state===2)
                    myPlayer.resume()
                else
                    myPlayer.pause()
            }
        }

        CoverAction {
            //enabled: queueList.currentIndex>0 && player.source!==""
            iconSource: "image://theme/icon-cover-next-song"
            onTriggered: {
                nowPlayingPage.nextSong()
            }
        }
    }

    CoverActionList {
        id: coverAction
        enabled: playerOpened && !showPrevCover

        CoverAction {
            //enabled: player.source!==""
            iconSource: myPlayer.state===2? "image://theme/icon-cover-play" : "image://theme/icon-cover-pause"
            onTriggered: {
                if (myPlayer.state===2)
                    myPlayer.resume()
                else
                    myPlayer.pause()
            }
        }

        CoverAction {
            //enabled: queueList.currentIndex>0 && player.source!==""
            iconSource: "image://theme/icon-cover-next-song"
            onTriggered: {
                nowPlayingPage.nextSong()
            }
        }
    }
}


