import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Page {
    id: root
    property bool pageloaded: false
    property bool mainloaded: false
    property bool startup: true

    allowedOrientations: appWindow.pagesOrientations

    canNavigateForward: database.loaded


    onStatusChanged: {
        if (status===PageStatus.Activating) {
            if (!pageloaded) {
                console.log("Loaded: " + pageloaded + " - main: " + mainloaded)
                utils.removeAlbumArt()

                misdatos.startup()

                totals = misdatos.dataInfo()
                artistsCovers = misdatos.getArtistsCovers()
                albumsCovers = misdatos.getAlbumsCovers()
                lastArtistItem = parseInt(utils.readSettings("LastArtistItem", "0"))
                lastAlbumItem = parseInt(utils.readSettings("LastAlbumItem", "0"))

                if (artistsCovers=="")
                    artistsCovers = "../artist.png"

                if (albumsCovers=="")
                    albumsCovers = "../album.png"

                artistsCoversModel.clear()
                for (var i=0; i<artistsCovers.split("<||>").length; ++i) {
                    artistsCoversModel.append({"url":artistsCovers.split("<||>")[i]})
                }

                albumsCoversModel.clear()
                for (var j=0; j<albumsCovers.split("<||>").length; ++j) {
                    albumsCoversModel.append({"url":albumsCovers.split("<||>")[j]})
                }

                misdatos.clearList()

                myPlaylists.clear()
                myplaylistmanager.loadPlaylists()

                lastGroup = "" //utils.readSettings("LastGroup", "albums")

                /*if (lastGroup=="albums")
                    misdatos.loadAlbums(utils.order)
                else if (lastGroup=="artists")
                    misdatos.loadArtists()
                else
                    misdatos.loadSongs(utils.readSettings("TrackOrder", "title"))*/

                pageloaded = true

                if (utils.cleanqueue==="yes")
                    myplaylistmanager.clearList("00000000000000000000")
            }

            /*if (!mainloaded) {
                mainloaded = true

                console.log("Pushing new attached: " + lastGroup)
                if (lastGroup==="artists" || lastGroup==="albums")
                    pageStack.pushAttached(mainPage)
                else
                    pageStack.pushAttached("SongsPage.qml")
            }*/

            if (radioModel.count===0) {
                radios.loadRadios()
            }

            if (startup) {
                startup = false
                //pageStack.navigateForward(PageStackAction.Immediate)
            }

            startTimers()

        }
        else if (status===PageStatus.Inactive)
        {
            stopTimers()
        }
    }

    function startTimers() {
        console.log("Starting timers")
        if (artistsCoversModel.count>1) {
            delegate1.startTimer()
            timer.start()
        }
    }

    function stopTimers() {
        utils.setSettings("LastArtistItem", delegate1.current + lastArtistItem)
        utils.setSettings("LastAlbumItem", delegate2.current  + lastAlbumItem)
        console.log("Stopping timers")
        delegate1.stopTimer()
        delegate2.stopTimer()
        timer.stop()
    }

    Connections {
        target: database
        onLoadChanged: {
            if (database.loaded) {
                misdatos.clearList()

                if (lastGroup=="albums")
                    misdatos.loadAlbums(utils.order)
                else if (lastGroup=="artists")
                    misdatos.loadArtists()
                else
                    misdatos.loadSongs(utils.readSettings("TrackOrder", "title"))
            }
        }
    }


    Header {
        visible: !database.loaded
        title: "FlowPlayer"
    }


    Column {
        visible: !database.loaded
        anchors.centerIn: parent
        width: parent.width -Theme.paddingLarge*2
        spacing: Theme.paddingLarge


        ProgressCircleBase {
            anchors.horizontalCenter: parent.horizontalCenter
            width: colSpace.width + Theme.paddingLarge*3
            height: width
            value: parseInt(database.perc) /100
            borderWidth: 2
            progressColor: Theme.highlightColor
            backgroundColor: Theme.rgba(Theme.secondaryHighlightColor, 0.5)

            Column {
                id: colSpace
                width: Theme.itemSizeMedium
                anchors.centerIn: parent

                Text {
                    id: percent
                    color: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeMedium
                    width: Theme.itemSizeMedium
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: database.perc + "%"
                }
            }

        }

        Label {
            width: parent.width
            text: qsTr("Updating music collection")
            horizontalAlignment: Text.AlignHCenter
            color: Theme.highlightColor
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Cancel")
            onClicked: database.cancelRead()
        }


    }

    SilicaFlickable {
        id: mainPanel
        anchors.fill: parent
        contentHeight: col1.height
        visible: database.loaded

        PullDownMenu {
            MenuItem {
                text: qsTr("Download album covers")
                onClicked: {
                    mainloaded = false
                    pageStack.push("FullAlbumSearch.qml")
                }
            }
            MenuItem {
                text: qsTr("Settings")
                onClicked: {
                    mainloaded = false
                    pageStack.push("Settings.qml")
                }
            }
            MenuItem {
                text: qsTr("Equalizer")
                onClicked: {
                    mainloaded = false
                    pageStack.push("Equalizer.qml")
                }
            }
        }

        Column {
            id: col1
            width: parent.width

            Header {
                title: "FlowPlayer"
            }

            Timer {
                id: timer
                interval: 2500
                repeat: false
                running: false
                onTriggered: {
                    if (albumsCoversModel.count>1)
                        delegate2.running = true
                }
            }

            StartDelegate {
                id: delegate1
                title: qsTr("Artists")
                count: totals.split(",")[1]
                model: artistsCoversModel
                onClicked: {
                    if (lastGroup!=="artists") {
                        utils.setSettings("LastGroup", "artists")
                        mainPage.searchValue = ""
                        mainPage.showSearch = false
                        mainPage.enableSearch = false
                        misdatos.clearList()
                        misdatos.loadArtists()
                        //if (lastGroup==="songs") {
                            console.log("Pushing attached: " + lastGroup)
                        //    pageStack.popAttached()
                            pageStack.pushAttached(mainPage)
                        //}
                        lastGroup = "artists"
                    }
                    pageStack.navigateForward()
                }
            }

            StartDelegate {
                id: delegate2
                title: qsTr("Albums")
                count: totals.split(",")[0]
                model: albumsCoversModel
                onClicked: {
                    if (lastGroup!=="albums") {
                        utils.setSettings("LastGroup", "albums")
                        mainPage.searchValue = ""
                        mainPage.showSearch = false
                        mainPage.enableSearch = false
                        misdatos.clearList()
                        misdatos.loadAlbums(utils.order)
                        //if (lastGroup==="songs") {
                            console.log("Pushing attached: " + lastGroup)
                        //    pageStack.popAttached()
                            pageStack.pushAttached(mainPage)
                        //}
                        lastGroup = "albums"
                    }
                    pageStack.navigateForward()
                }
            }

            StartDelegate {
                title: qsTr("Tracks")
                model: ListModel { ListElement {url:"../tracks.png"} }
                count: totals.split(",")[2]
                onClicked: {
                    if (lastGroup!=="songs") {
                        utils.setSettings("LastGroup", "songs")
                        lastGroup = "songs"
                        misdatos.clearList()
                        misdatos.loadSongs(utils.readSettings("TrackOrder", "title"))
                        console.log("Pushing attached: " + lastGroup)
                        //pageStack.popAttached()
                        pageStack.pushAttached("SongsPage.qml")
                    }
                    pageStack.navigateForward()
                }
            }

            StartDelegate {
                title: qsTr("Playlists")
                model: ListModel { ListElement {url:"../playlist.png"} }
                count: myPlaylists.count
                onClicked: {
                    mainloaded = false
                    pageStack.push("Playlists.qml")
                }
            }

            StartDelegate {
                title: qsTr("Radio stations")
                model: ListModel { ListElement {url:"../radio.png"} }
                count: radioModel.count
                onClicked: {
                    mainloaded = false
                    pageStack.push("OnlineRadios.qml")
                }
            }

            /*Item {
                height: Theme.itemSizeSmall/2
                width: parent.width
                Separator {
                    width: parent.width-Theme.paddingLarge*2
                    anchors.centerIn: parent
                    color: Theme.highlightColor
                }
            }


            SimpleDelegate {
                title: qsTr("Download album covers")
                iconimage: "image://theme/icon-m-health"
                onClicked: {
                    mainloaded = false
                    pageStack.push("FullAlbumSearch.qml")
                }
            }

            SimpleDelegate {
                title: qsTr("Settings")
                iconimage: "image://theme/icon-m-developer-mode"
                onClicked: {
                    mainloaded = false
                    pageStack.push("Settings.qml")
                }
            }

            SimpleDelegate {
                title: qsTr("Equalizer")
                iconimage: "image://theme/icon-m-accessory-speaker"
                onClicked: {
                    mainloaded = false
                    pageStack.push("Equalizer.qml")
                }
            }*/
        }

    }

}

