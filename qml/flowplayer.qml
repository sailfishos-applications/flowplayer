import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0
import Amber.Mpris 1.0
import com.jolla.mediaplayer 1.0
import "pages"

ApplicationWindow
{
    cover: CoverPage { id: coverPage }

    id: appWindow
    initialPage: startPage

    onApplicationActiveChanged: {
        if (appWindow.applicationActive) {
            startPage.startTimers()
        } else {
            startPage.stopTimers()
        }
    }

    property int lastArtistItem
    property int lastAlbumItem

    property string savedorientation: utils.readSettings("Orientation", "auto")

    property int pagesOrientations: savedorientation==="auto"? (Orientation.All) :
                                    (savedorientation==="landscape"? Orientation.Landscape : Orientation.Portrait)


    property string currentPlaylist

    property bool playingRadio: false
    property bool shuffle: false
    property bool repeat: false

    property bool showPrevCover: utils.readSettings("ShowPreviousButton", "no")==="yes"
    property bool showBigCover: utils.readSettings("BigCoverBackground", "no")==="yes"

    signal setLanguage(string langval)

    signal playerDurationChanged(int duration)
    signal playerPositionChanged(int position)
    signal playerSourceChanged(string source)

    property string totals;
    property string artistsCovers;
    property string albumsCovers;
    ListModel { id: artistsCoversModel }
    ListModel { id: albumsCoversModel }


    function favAdded() {
        if (myPlaylists.count>1) {
            var val = myPlaylists.get(1).count
            if (val==="") val = 1
            else val = parseInt(val) +1
            myPlaylists.setProperty(1, "count", val.toString())
        }
    }

    function favRemoved() {
        if (myPlaylists.count>1) {
            var val = myPlaylists.get(1).count
            if (val==="1") val = ""
            else val = parseInt(val) -1
            myPlaylists.setProperty(1, "count", val.toString())
        }
    }

    //signal metadataChanged(string artist, string album)
    signal albumMetadataChanged(string artistold, string artistnew,
                                string albumold, string albumnew,
                                string titleold, string titlenew)

    property variant currentSongInfo: []

    property string currentListOrder: utils.order

    property string lastGroup


    property string filterArtist
    property string filterAlbum
    property string filterArtistCount
    property string filterSong

    property bool gaplessPlayback: utils.readSettings("GaplessPlayback", "no")==="yes"

    Player {
        id: myPlayer

        onSourceChanged: {
            playerSourceChanged(myPlayer.source)
        }

        onStateChanged: {
            if (myPlayer.state===0) {
                if (queueList.count==1)
                    nowPlayingPage.changeSong()
                else
                    nowPlayingPage.nextSong()
            }
        }

        onDurationChanged: {
            playerDurationChanged(myPlayer.duration)
        }

        onPositionChanged: {
            mprisPlayer.position = myPlayer.position
            playerPositionChanged(myPlayer.position)
        }

        /*onAboutToFinish: {
            if (gaplessPlayback) {
                var next = nowPlayingPage.getNextSong()
                myPlayer.setSource(next, false)
            }
        }*/

    }



    /*Audio {
        id: player
        source: ""
        property int stat: playbackState
        property int stat2: player.status

        onDurationChanged: {
            playerDurationChanged(player.source, duration)
        }
        onPositionChanged: {
            mprisPlayer.position = position *1000
            playerPositionChanged(player.source, position)
        }

        onStatusChanged: {
            if (status == Audio.EndOfMedia) {
                if (queueList.count==1)
                    nowPlayingPage.changeSong()
                else
                    nowPlayingPage.nextSong()
            }
        }

        onSourceChanged: {
            playerSourceChanged(source)
        }

    }*/

    onCurrentSongInfoChanged: {
        var metadata
        if (currentSongInfo) {
            metadata = {
                'url'       : currentSongInfo.url,
                'title'     : currentSongInfo.title ? currentSongInfo.title : qsTr("(radio)"),
                'artist'    : currentSongInfo.artist ? currentSongInfo.artist : currentSongInfo.name,
                'album'     : currentSongInfo.album,
                'genre'     : "",
                'track'     : queueList.currentIndex,
                'trackCount': queueList.count,
                'duration'  : currentSongInfo.duration * 1000,
                'imageUrl'  : currentSongInfo.imageurl ? currentSongInfo.imageurl : utils.thumbnail(currentSongInfo.artist, currentSongInfo.album),
            }
        }
        mprisPlayer.localMetadata = metadata
        bluetoothMediaPlayer.metadata = metadata
    }


    StartPage { id: startPage }
    MainPage { id: mainPage }

    Database { id: database }
    Meta { id: meta }
    Utils { id: utils }
    CoverSearch { id: coversearch }
    MyPlaylistManager { id: myplaylistmanager }
    Missing { id: missing }
    Datos { id: misdatos } //NOSPARQL
    LFM { id: lfm }
    Radios { id: radios }


    /*Connections {
        target: missing
        onAlbumCoverChanged: {
            console.log("Updated cover for " + coverpath)
            misdatos.reloadItem(coverpath)
        }
    }*/


    ListModel {
        id: queueList
        property int currentIndex
    }

    ListModel {
        id: helperList
        property int currentIndex
    }

    ListModel {
        id: helperList2
        property int currentIndex
    }

    ListModel {
        id: myPlaylists
    }

    ListModel {
        id: radioModel
    }

    Banner { id: ibanner }

    Connections {
        target: myplaylistmanager

        onAddPlaylist: {
            //console.log("Adding playlist: " + list.name + " - " + list.count + " - " + list.type)
            myPlaylists.append({"name":list.name, "count":list.count, "type":list.type})
        }

        onAddItemToList: {
            if (list==="00000000000000000000") {
                //console.log("Adding to queue: " + item.title)
                queueList.append({"artist":item.artist, "album":item.album, "title":item.title,
                                   "duration":item.duration, "url":item.url})

            }
        }

        onAddItemToListDone: {
            if (list==="00000000000000000000")
                utils.setShuffle(queueList.count)
        }

    }

    Connections {
        target: radios

        onAppendRadio: {
            radioModel.append({"name":name, "url":genre, "radioid":radioid, "image":image})
        }
    }

    NowPlaying {
        id: nowPlayingPage

        onStatusChanged: {
            if (status===PageStatus.Activating)
                miniPlayer.open = false
            else if (status===PageStatus.Deactivating)
                miniPlayer.open = true
        }

    }

    Lyrics {
        id: lyricsPage
    }

    //property real miniPlayerSize: appWindow.height -miniPlayer.y

    bottomMargin: miniPlayer.visibleSize

    property bool playerOpened: miniPlayer.open || nowPlayingPage.status===PageStatus.Active

    function closeMiniplayer() {
        miniPlayer.open = false
    }

    property bool nppOpened: false

    DockedPanel {
        id: miniPlayer
        dock: Dock.Bottom
        width: parent.width
        height: Theme.itemSizeExtraLarge
        opacity: open && !Qt.inputMethod.visible? 1 : 0
        Behavior on opacity { FadeAnimation {} }
        open: false
        contentHeight: height
        flickableDirection: Flickable.VerticalFlick
        //z: 1

        onOpenChanged: {
            if (!open && nowPlayingPage.status===PageStatus.Inactive) {
                myPlayer.stop()
                myPlayer.setSource("")
                utils.removeAlbumArt()
                currentSongInfo = []
            }
        }

        Image {
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: "image://theme/graphic-gradient-edge"
        }

        Item {
            id: miniPlayerControls
            anchors.fill: parent

            CoverArtList {
                id: thumb
                x: 0
                width: parent.height
                height: width
                itemimg: playingRadio? currentSongInfo.imageurl : utils.thumbnail(currentSongInfo.artist, currentSongInfo.album)
                text: qsTr("Cover not found")
                onClicked: {
                    if (nppOpened)
                        pageStack.navigateBack()
                    else
                        pageStack.push(nowPlayingPage)
                }
            }

            Column {
                spacing: Theme.paddingSmall
                anchors.left: thumb.right
                width: parent.width - thumb.width
                anchors.verticalCenter: parent.verticalCenter


                Label {
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.paddingLarge
                    width: parent.width - Theme.paddingLarge*2
                    text: playingRadio? "<font color=\"" + Theme.highlightColor + "\">" + (currentSongInfo.artist!==""?
                                        currentSongInfo.artist : currentSongInfo.name) + "</font> " + currentSongInfo.title :
                          "<font color=\"" + Theme.highlightColor + "\">" + currentSongInfo.artist + "</font> " + currentSongInfo.title
                    font.pixelSize: Theme.fontSizeSmall
                    truncationMode: TruncationMode.Fade
                    textFormat: Text.RichText
                    //onTextChanged: console.log("NEW TEXT: " + text)
                    //horizontalAlignment: Text.AlignHCenter
                }


                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: parent.width/7

                    IconButton {
                        icon.source: "image://theme/icon-m-previous"
                        enabled: queueList.count>1
                        onClicked: {
                            nowPlayingPage.prevSong()
                            //nowPlayingPage.changeSong()
                        }
                    }

                    IconButton {
                        icon.source: myPlayer.state===2? "image://theme/icon-m-play" : "image://theme/icon-m-pause"
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
                        onClicked: {
                            nowPlayingPage.nextSong()
                            //nowPlayingPage.changeSong()
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


    MprisPlayer {
        id: mprisPlayer

        property var localMetadata

        function emitSeeked() {
            seeked(player.position * 1000)
        }

        serviceName: "flowplayer"

        // Mpris2 Root Interface
        identity: "FlowPlayer"
        desktopEntry: "flowplayer"
        supportedUriSchemes: ["file", "http", "https"]
        supportedMimeTypes: ["audio/x-wav", "audio/mp4", "audio/mpeg", "audio/x-vorbis+ogg", "audio/ogg", "audio/opus"]

        // Mpris2 Player Interface
        canControl: currentSongInfo !== []
        canGoNext: queueList.count>1 && currentSongInfo!==[]
        canGoPrevious: queueList.count>1 && currentSongInfo!==[]
        canPause: queueList.count>0 && currentSongInfo!==[]
        canPlay: queueList.count>0 && currentSongInfo!==[]
        canSeek: queueList.count>0 && myPlayer.state>0

        //loopStatus: repeatSwitch.checked ? Mpris.Playlist : Mpris.None
        playbackStatus: {
            if (myPlayer.state===2) {
                return Mpris.Paused
            } else if (myPlayer.state===1) {
                return Mpris.Playing
            } else {
                return Mpris.Stopped
            }
        }
        //position: player.position * 1000
        shuffle: false //shuffleSwitch.checked
        volume: 1

        onPauseRequested: myPlayer.pause()
        onPlayRequested: myPlayer.resume()
        onPlayPauseRequested: {
            if (myPlayer.state===2)
                myPlayer.resume()
            else
                myPlayer.pause()
        }
        onStopRequested: myPlayer.stop()

        // This will start playback in any case. Mpris says to keep
        // paused/stopped if we were before but I suppose this is just
        // our general behavior decision here.
        onNextRequested: nowPlayingPage.nextSong()
        onPreviousRequested: nowPlayingPage.prevSong()

        onSeekRequested: {
            var position = myPlayer.position + offset
            myPlayer.seek(position < 0 ? 0 : position)
            emitSeeked()
        }
        onSetPositionRequested: {
            myPlayer.seek(position)
            emitSeeked()
        }
        //onOpenUriRequested: playUrl(url)

        onPositionRequested: {
            mprisPlayer.position = player.position * 1000
        }

        /*onLoopStatusRequested: {
            if (loopStatus == Mpris.None) {
                repeatSwitch.checked = false
            } else if (loopStatus == Mpris.Playlist) {
                repeatSwitch.checked = true
            }
        }*/
        //onShuffleRequested: shuffleSwitch.checked = shuffle

        onLocalMetadataChanged: {
            if (localMetadata && 'url' in localMetadata) {
                mprisPlayer.metaData.url = localMetadata.url
                mprisPlayer.metaData.trackId = "/com/jolla/mediaplayer/" + Qt.md5(localMetadata['url'].toString()) // DBus object path
                mprisPlayer.metaData.duration = localMetadata.duration // milliseconds
                mprisPlayer.metaData.albumTitle = localMetadata.album
                mprisPlayer.metaData.albumArtist = localMetadata.artist
                mprisPlayer.metaData.contributingArtist = localMetadata.artist
                mprisPlayer.metaData.genre = [localMetadata.genre] // List of strings
                mprisPlayer.metaData.title = localMetadata.title
                mprisPlayer.metaData.trackNumber = localMetadata.track
                mprisPlayer.metaData.artUrl = localMetadata.imageUrl
            }
        }
    }

    MyMediaKeys {
        id: mediaKeys
    }

    BluetoothMediaPlayer {
        id: bluetoothMediaPlayer

        status: {
            if (myPlayer.state===1) {
                return BluetoothMediaPlayer.Playing
            } else if (myPlayer.state===2) {
                return BluetoothMediaPlayer.Paused
            } else {
                return BluetoothMediaPlayer.Stopped
            }
        }

        onStatusChanged: console.log("BT PLAYBACK STATUS: " + status + " - Player state: " + myPlayer.state)

        repeat: appWindow.repeat
                    ? BluetoothMediaPlayer.RepeatAllTracks
                    : BluetoothMediaPlayer.RepeatOff

        shuffle: appWindow.shuffle
                    ? BluetoothMediaPlayer.ShuffleAllTracks
                    : BluetoothMediaPlayer.ShuffleOff

        position: myPlayer.position

        metadata: {} //currentSongInfo

        onChangeRepeat: {
            if (repeat == BluetoothMediaPlayer.RepeatOff) {
                appWindow.repeat = false
            } else if (repeat == BluetoothMediaPlayer.RepeatAllTracks) {
                appWindow.repeat = true
            }
        }

        onChangeShuffle: {
            if (shuffle == BluetoothMediaPlayer.ShuffleOff) {
                appWindow.shuffle = false
            } else if (shuffle == BluetoothMediaPlayer.ShuffleAllTracks) {
                appWindow.shuffle = true
            }
        }
    }


    function replaceText(text, str) {
        var ltext = text.toLowerCase()
        var lstr = str.toLowerCase()
        var ind = ltext.indexOf(lstr)
        if (ind<0 || str==="") return text
        var txt = text.substring(0,ind)
        var ntext = txt + "<font color=" + Theme.highlightColor + ">" +
                    text.slice(ind, ind+str.length)  + "</font>" +
                    text.slice(ind+str.length, text.length);
        return ntext;
    }

}


