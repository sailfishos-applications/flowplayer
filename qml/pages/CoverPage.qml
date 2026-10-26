import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

CoverBackground {
    id: cover

    // Nothing playing: random album covers, like the Jolla media player
    property var idleCovers: []
    property bool showIdleCovers: !playerOpened && idleCovers.length > 0

    function pickIdleCovers() {
        var covers = albumsCovers.split("<||>").filter(function(c) { return c !== "" && c !== "../album.png" })
        var picked = []
        while (covers.length > 0 && picked.length < 3)
            picked.push(covers.splice(Math.floor(Math.random() * covers.length), 1)[0])
        idleCovers = picked
    }

    // Filled once the library is loaded
    property string allAlbumsCovers: albumsCovers
    onAllAlbumsCoversChanged: pickIdleCovers()

    onStatusChanged: {
        if (status === Cover.Activating && !playerOpened)
            pickIdleCovers()
    }

    Item {
        id: idleCollage
        anchors.fill: parent
        visible: showIdleCovers

        Image {
            id: largeIdleCover
            source: idleCovers.length > 0 ? idleCovers[0] : ""
            width: parent.width
            height: idleCovers.length > 1 ? width : parent.height
            sourceSize.width: width
            sourceSize.height: height
            fillMode: Image.PreserveAspectCrop
        }

        Row {
            visible: idleCovers.length > 1
            anchors.top: largeIdleCover.bottom
            anchors.bottom: parent.bottom
            opacity: Theme.opacityLow

            Repeater {
                model: idleCovers.slice(1)
                Image {
                    source: modelData
                    width: cover.width / (idleCovers.length - 1)
                    height: parent.height
                    sourceSize.width: width
                    sourceSize.height: height
                    fillMode: Image.PreserveAspectCrop
                }
            }
        }
    }

    // Change the covers every few seconds while the cover is shown
    Timer {
        interval: 6000
        repeat: true
        running: showIdleCovers && cover.status === Cover.Active
        onTriggered: changeIdleCovers.start()
    }

    SequentialAnimation {
        id: changeIdleCovers
        FadeAnimation { target: idleCollage; to: 0 }
        ScriptAction { script: pickIdleCovers() }
        FadeAnimation { target: idleCollage; to: 1 }
    }

    CoverPlaceholder {
        visible: !playerOpened && !showIdleCovers
        text: "FlowPlayer"
        icon.source: "file:///usr/share/icons/hicolor/86x86/apps/flowplayer.png"
        icon.width: Theme.iconSizeLauncher
        icon.height: Theme.iconSizeLauncher
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


