import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0
import "dateandtime.js" as DT


Page {
    id: root

    allowedOrientations: appWindow.pagesOrientations

    property bool started: false
    property bool needscroll: true
    property bool isFav: false

    //backNavigation: false

    /*property string currentSongArtist
    property string currentSongAlbum
    property string currentSongTitle
    property string prevSongArtist
    property string prevSongAlbum
    property string nextSongArtist
    property string nextSongAlbum*/

    property bool radioExists: false

    onStatusChanged: {
        if (status===PageStatus.Activating) {
            bigCoverList.positionViewAtIndex(queueList.currentIndex,PathView.Center)
            nppOpened = false
        }
        else if (status===PageStatus.Active && !playingRadio) {
            if (currentSongInfo.artist!==qsTr("Unknown artist") || currentSongInfo.album!==qsTr("Unknown album"))
                pageStack.pushAttached(lyricsPage)
        }
        else if (status===PageStatus.Active && playingRadio) {
            console.log("Current radio: " + currentSongInfo.radioid)
            radioExists = radios.radioExists(currentSongInfo.name, currentSongInfo.url)
        }
        else if (status===PageStatus.Inactive) {
            dockPanel.open = false
        }
    }

    Connections {
        target: radios

        onPlayInfoLoaded: {
            if (!playingRadio)
                return

            console.log("Radio song loaded: " + artist + " - " + title + " - " + remaining)

            currentSongInfo = {name:currentSongInfo.name, url:currentSongInfo.url, radioid:currentSongInfo.radioid,
                imageurl:currentSongInfo.imageurl, artist:artist, album:"", title:title}

            if (artist==="" &&  title==="")
                return;

            if (remaining==0)
                remaining = 60

            timer.current = currentSongInfo.name
            timer.interval = remaining *1000
            timer.restart()
        }
    }

    Timer {
        id: timer
        property string current
        repeat: false
        running: false
        triggeredOnStart: false
        onTriggered: {
            timer.stop()
            if (playingRadio && current===currentSongInfo.radioid) {
                radios.getPlayingInfo(currentSongInfo.radioid)
            }
        }
    }

    Connections {
        target: appWindow

        onPlayerSourceChanged: {
            if (source!=="") {
                if (playingRadio || bigCoverList.count<1)
                    return;

                if (source!==bigCoverList.currentItem.myData.url)
                {
                    console.log("Moving flowlist to correct url: " + source)
                    for ( var i=0; i<queueList.count; ++i ) {
                        if (queueList.get(i).url===source) {
                            bigCoverList.positionViewAtIndex(i, PathView.Center)
                            queueList.currentIndex = i
                            songsList.currentIndex = i
                            break;
                        }
                    }
                }

                utils.createAlbumArt(utils.thumbnail(bigCoverList.currentItem.myData.artist, bigCoverList.currentItem.myData.album))

                var prevArtist = currentSongInfo.artist
                currentSongInfo = {artist:bigCoverList.currentItem.myData.artist,
                                   album:bigCoverList.currentItem.myData.album,
                                   title:bigCoverList.currentItem.myData.title,
                                   url:bigCoverList.currentItem.myData.url,
                                   duration:bigCoverList.currentItem.myData.duration
                                  }

                console.log("Current song info changed: " + currentSongInfo.artist + " - " +
                            currentSongInfo.album + " - " + currentSongInfo.title)

                if (currentSongInfo.artist!==qsTr("Unknown artist") || currentSongInfo.album!==qsTr("Unknown album"))
                {
                    lyricsPage.reloadLyrics()
                    if (currentSongInfo.artist !== prevArtist)
                        lyricsPage.reloadInfo()
                }

                var next = getNextSong()
                myPlayer.setNextSource(next, false)

                isFav = utils.isFav(bigCoverList.currentItem.myData.url)

            }
            else {
                utils.removeAlbumArt()
                currentSongInfo = []
                lyricsPage.clearLyrics()
            }
        }

    }

    function playSong(index) {
        console.log("Change index to " + index + " - count: " + bigCoverList.count)
        bigCoverList.positionViewAtIndex(index, PathView.Center)
        changeSong()
    }


    function changeSong() {
        playingRadio = false
        console.log("Playing " + bigCoverList.currentItem.myData.title)
        queueList.currentIndex = bigCoverList.currentIndex
        songsList.currentIndex = bigCoverList.currentIndex
        //player.source = "file://" + decodeURIComponent(bigCoverList.currentItem.myData.url)
        //player.play()
        myPlayer.setSource(decodeURIComponent(bigCoverList.currentItem.myData.url))
        myPlayer.play()
    }

    function nextSong() {

        if (repeat) {
            player.stop()
            player.play()
            return
        }

        if (shuffle) {
            playSong(utils.getShuffleTrack(bigCoverList.currentIndex))
            return
        }

        if (bigCoverList.count===1) return;
        if (bigCoverList.currentIndex==bigCoverList.count-1)
            bigCoverList.positionViewAtIndex(0, PathView.Center)
        else
            bigCoverList.positionViewAtIndex(bigCoverList.currentIndex+1, PathView.Center)
        changeSong()
    }

    function getNextSong() {
        if (repeat || bigCoverList.count===1) {
            return decodeURIComponent(bigCoverList.currentItem.myData.url)
        }
        if (shuffle) {
            return decodeURIComponent(queueList.get(bigCoverList.currentIndex).url)
        }
        if (bigCoverList.currentIndex==bigCoverList.count-1)
            return decodeURIComponent(queueList.get(0).url)
        else
            return decodeURIComponent(queueList.get(bigCoverList.currentIndex+1).url)
    }

    function prevSong() {

        if (repeat) {
            player.stop()
            player.play()
            return
        }

        if (shuffle) {
            playSong(utils.getShuffleTrack(bigCoverList.currentIndex))
            return
        }

        if (bigCoverList.count===1) return;
        if (bigCoverList.currentIndex==0)
            bigCoverList.positionViewAtIndex(bigCoverList.count-1, PathView.Center)
        else
            bigCoverList.positionViewAtIndex(bigCoverList.currentIndex-1, PathView.Center)
        changeSong()
    }

    function reloadMetadata() {

    }

    property bool showList: false

    Connections {
        target: appWindow

        onAlbumMetadataChanged: {
            for ( var i=0; i<queueList.count; ++i )
            {
                if (queueList.get(i).artist===artistold &&
                    queueList.get(i).album===albumold &&
                    queueList.get(i).title===titleold) {

                    queueList.get(i).artist===artistnew
                    queueList.get(i).album===albumnew
                    queueList.get(i).title===titlenew

                    if (bigCoverList.currentIndex==i) {
                        currentSongInfo = {artist:bigCoverList.currentItem.myData.artist,
                                           album:bigCoverList.currentItem.myData.album,
                                           title:bigCoverList.currentItem.myData.title,
                                           url:bigCoverList.currentItem.myData.url,
                                           duration:bigCoverList.currentItem.myData.duration
                                          }
                        lyricsPage.reloadLyrics()
                    }
                    break;
                }
            }
        }
    }


    SilicaFlickable {
        id: flickArea
        anchors.fill: parent
        contentHeight: bigCoverList.height + (root.isPortrait? col1.height : 0) // + Theme.paddingLarge
        clip: true

        PullDownMenu {

            MenuItem {
                text: qsTr("Equalizer")
                onClicked: {
                    nppOpened = true
                    pageStack.push("Equalizer.qml")
                }
            }
            MenuItem {
                visible: !playingRadio
                text: qsTr("Edit metadata")
                onClicked: {
                    nppOpened = true
                    helperList2.clear()
                    helperList2.append({"url":bigCoverList.currentItem.myData.url})
                    meta.setFile(decodeURIComponent(helperList2.get(0).url))
                    pageStack.push ("Metadata.qml", {"artist":bigCoverList.currentItem.myData.artist,
                                        "album":bigCoverList.currentItem.myData.album,
                                        "title":bigCoverList.currentItem.myData.title
                                    })
                }
            }
            MenuItem {
                visible: !playingRadio
                text: isFav? qsTr("Remove from favorites") : qsTr("Add to favorites")
                onClicked: {
                    if (!isFav) favAdded()
                    else favRemoved()
                    utils.favSong(bigCoverList.currentItem.myData.url, !isFav)
                    isFav = utils.isFav(bigCoverList.currentItem.myData.url)
                }
            }
            MenuItem {
                visible: !playingRadio
                text: qsTr("Add to playlist")
                onClicked: {
                    nppOpened = true
                    helperList2.clear()
                    helperList2.append({"artist":bigCoverList.currentItem.myData.artist,
                                        "album":bigCoverList.currentItem.myData.album,
                                        "title":bigCoverList.currentItem.myData.title,
                                        "url":bigCoverList.currentItem.myData.url,
                                        "duration":bigCoverList.currentItem.myData.duration
                                      })
                    pageStack.push("SelectPlaylist.qml")
                }
            }
            MenuItem {
                visible: playingRadio && currentSongInfo.radioid!==""
                text: qsTr("Reload info")
                onClicked: {
                    timer.stop()
                    radios.getPlayingInfo(currentSongInfo.radioid)
                }
            }
            MenuItem {
                visible: playingRadio
                enabled: !radioExists
                text: qsTr("Save station")
                onClicked: {
                    radios.saveRadio(currentSongInfo.name, currentSongInfo.url, currentSongInfo.radioid, currentSongInfo.imageurl)
                    radioExists = true
                    radioModel.clear()
                    radios.loadRadios()
                }
            }
        }


        /*SilicaListView {
            id: bigCoverList
            x: 0; y:0
            width: root.isPortrait? root.width : root.height
            height: width
            clip: true
            orientation: ListView.Horizontal
            snapMode: ListView.SnapOneItem
            highlightRangeMode : ListView.StrictlyEnforceRange
            highlightMoveVelocity: 1500

            model: queueList

            delegate: CoverArtList {
                property variant myData: model
                width: root.isPortrait? root.width : root.height
                height: width
                clip: true
                itemimg: utils.thumbnail(model.artist, model.album)
                text: qsTr("Cover not found")
                textSize: Theme.fontSizeLarge

                onClicked: {
                    dockPanel.open = !dockPanel.open
                }
            }

            onMovementEnded: {
                //if (root.status!==PageStatus.Active)
                //    return
                changeSong()
            }

        }*/

        PathView {
            id: bigCoverList
            x: 0; y: 0
            width: root.isPortrait? root.width : root.height
            height: width
            clip: true
            //orientation: ListView.Horizontal
            snapMode: ListView.SnapOneItem
            highlightRangeMode : ListView.StrictlyEnforceRange
            //highlightMoveVelocity: 1500
            enabled: !playingRadio


            preferredHighlightBegin: 0.5
            preferredHighlightEnd: 0.5

            pathItemCount: 3

            path: Path {
                startX: -bigCoverList.width
                startY: bigCoverList.height / 2
                PathLine { x: bigCoverList.width; y: bigCoverList.height / 2;  }
                PathLine { x: bigCoverList.width *2; y: bigCoverList.height / 2; }
            }

            model: queueList

            delegate: CoverArtList {
                property variant myData: model
                width: root.isPortrait? root.width : root.height
                height: width
                clip: true
                itemimg: playingRadio? currentSongInfo.imageurl : utils.thumbnail(model.artist, model.album)
                text: playingRadio? currentSongInfo.name : qsTr("Cover not found")
                textSize: Theme.fontSizeLarge

                onClicked: {
                    songsList.positionViewAtIndex(bigCoverList.currentIndex, ListView.Center)
                    dockPanel.open = true
                }
            }

            onMovementEnded: {
                if (bigCoverList.currentIndex==queueList.currentIndex )
                    return
                changeSong()
            }

        }

        /*IconButton {
            anchors.right: parent.right
            //anchors.rightMargin: Theme.paddingMedium
            anchors.bottom: bigCoverList.bottom
            icon.source: "image://theme/icon-l-down"
            onClicked: pageStack.pop()
        }*/

        Column {
            id: col1
            spacing: root.isPortrait? Theme.paddingSmall : Theme.paddingMedium
            anchors.top: root.isPortrait? bigCoverList.bottom : parent.top
            anchors.topMargin: root.isPortrait? Theme.paddingMedium : Theme.paddingLarge
            anchors.left: root.isPortrait? parent.left : bigCoverList.right
            anchors.right: parent.right

            Label {
                id: txtSong
                //visible: root.isLandscape
                font.pixelSize: Theme.fontSizeMedium
                width: parent.width - Theme.paddingLarge*2
                anchors.left: parent.left
                anchors.leftMargin: Theme.paddingLarge
                text: currentSongInfo.title!==""? currentSongInfo.title : currentSongInfo.name
                truncationMode: TruncationMode.Fade
                color: Theme.highlightColor
            }

            Label {
                id: txtArtist
                font.pixelSize: Theme.fontSizeSmall
                width: parent.width - Theme.paddingLarge*2
                anchors.left: parent.left
                anchors.leftMargin: Theme.paddingLarge
                text: currentSongInfo.artist
                truncationMode: TruncationMode.Fade
            }

            Label {
                id: txtAlbum
                font.pixelSize: Theme.fontSizeSmall
                width: parent.width - Theme.paddingLarge*2
                anchors.left: parent.left
                anchors.leftMargin: Theme.paddingLarge
                text: currentSongInfo.album
                truncationMode: TruncationMode.Fade
            }

            Item {
                width: parent.width
                height: root.isPortrait? Theme.paddingLarge : Theme.itemSizeExtraSmall
                //visible: root.isLandscape
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: parent.width/5

                IconButton {
                    icon.source: "image://theme/icon-m-previous"
                    enabled: queueList.count>1
                    onClicked: prevSong()
                }

                IconButton {
                    icon.source: myPlayer.state===2? "image://theme/icon-l-play" : "image://theme/icon-l-pause"
                    onClicked: {
                        if (myPlayer.state===2)
                            myPlayer.resume()
                        else
                            myPlayer.pause()
                    }
                }

                IconButton {
                    icon.source: "image://theme/icon-m-next"
                    enabled: queueList.count>1
                    onClicked: nextSong()
                }

            }

            Slider {
                id: positionSlider

                //height: Theme.itemSizeMedium

                width: parent.width - Theme.paddingLarge*2
                anchors.horizontalCenter: parent.horizontalCenter
                minimumValue: 0
                maximumValue: playingRadio? 0 : myPlayer.duration
                valueText: playingRadio? "" : DT.getDuration(parseInt(value))
                label: playingRadio? qsTr("Online radio") : (myPlayer.duration>-1? DT.getDuration(myPlayer.duration) : "")

                onReleased: myPlayer.seek(value)

                Connections {
                    target: appWindow

                    onPlayerPositionChanged: {
                        if (!playingRadio && !positionSlider.pressed)
                            positionSlider.value = position
                    }

                    onPlayerDurationChanged: {
                        positionSlider.maximumValue = duration>-1? duration : 0

                        if (parseInt(bigCoverList.currentItem.myData.duration)!==duration && duration>-1) {
                            console.log("Database duration is " + parseInt(bigCoverList.currentItem.myData.duration) +
                                        " and player says it's " + duration + ". Updating database")
                            utils.updateSongDuration(bigCoverList.currentItem.myData.url, duration)
                        }
                    }
                }

            }

        }

        PushUpMenu {
            Row {
                width: parent.width

                Switch {
                    id: shuffleMenu
                    width: parent.width * 0.5
                    icon.source: "image://theme/icon-m-shuffle"
                    checked: shuffle
                    onClicked: shuffle = !shuffle
                }

                Switch {
                    id: repeatMenu
                    width: parent.width * 0.5
                    icon.source: "image://theme/icon-m-repeat"
                    checked: repeat
                    onClicked: repeat = !repeat
                }
            }
        }

    }

    DockedPanel {
        id: dockPanel
        dock: Dock.Top
        width: bigCoverList.width
        height: width //root.isPortrait? (root.height - Theme.paddingLarge*2 + Theme.itemSizeMedium) : root.height
        open: false
        opacity: open? 1 : 0
        clip: true

        Behavior on opacity { FadeAnimation {} }

        Rectangle {
            anchors.fill: parent
            color: Qt.darker(Theme.highlightColor, 2.5)
            opacity: 0.9
        }


        SilicaListView {
            id: songsList
            anchors.fill: parent
            anchors.bottomMargin: Theme.itemSizeMedium/2 + Theme.paddingMedium
            clip: true

            model: queueList

            delegate: AlbumDelegate {
                myData: model
                name: model.title
                desc: model.artist
                img: utils.thumbnail(model.artist, model.album)
                time: DT.getDuration(model.duration)
                cindex: index
                //count: model.count
                isplaying: decodeURIComponent(model.url) === decodeURIComponent(myPlayer.source)
                contentHeight: Theme.itemSizeSmall

                onClicked: {
                    bigCoverList.positionViewAtIndex(model.index,PathView.Center)
                    dockPanel.open = false
                    changeSong()
                }
            }

        }
        IconButton {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: -Theme.paddingSmall
            anchors.horizontalCenter: parent.horizontalCenter
            icon.source: "image://theme/icon-l-up"
            icon.height: Theme.itemSizeMedium/2
            icon.width: icon.height
            onClicked: dockPanel.open = false
        }
    }

}
