import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Page {
    id: root

    allowedOrientations: appWindow.pagesOrientations

    property string artist
    property int totalSongs: 0

    property bool loaded: false

    onStatusChanged: {
        if (status===PageStatus.Activating && !loaded) {
            console.log("Loading albums: " + artist)
            albumList.clear()
            myplaylistmanager.loadArtist(artist)
            loaded = true
        }
    }

    ListModel { id: albumList }

    Connections {
        target: myplaylistmanager

        onAddItemToArtist: {
            albumList.append({"artist":item.artist, "album":item.album, "songs":item.songs})
        }

        onArtistLoaded: {
            totalSongs = totalsongs
        }

    }

    SilicaListView {
        id: songlist
        anchors.fill: parent
        clip: true

        PullDownMenu {
            MenuItem {
                text: qsTr("Search artist image")
                enabled: artist!==qsTr("Unknown artist")
                onClicked: {
                    coversearch.clearData()
                    pageStack.push ("CoverDownload.qml", {"artist":artist, "searchingArtist":true, "album":""})
                }
            }

            MenuItem {
                text: qsTr("Add to playlist")
                onClicked: {
                    filterArtist = artist
                    filterAlbum = ""
                    filterArtistCount = "1"
                    filterSong = ""
                    pageStack.push("SelectPlaylist.qml")
                }
            }
            MenuItem {
                text: qsTr("Play all")
                onClicked: {
                    queueList.clear()
                    myplaylistmanager.clearList("00000000000000000000")
                    myplaylistmanager.addAlbumToList("00000000000000000000", artist, "", "1", "")
                    myplaylistmanager.loadPlaylist("00000000000000000000")
                    queueList.currentIndex = 0
                    nowPlayingPage.playSong(0)
                    miniPlayer.open = true
                }
            }
        }

        model: albumList

        header: mainColumn

        delegate: AlbumDelegate {
            myData: model
            name: model.album
            desc: model.songs==="1" ? qsTr("1 track") : qsTr("%1 tracks").arg(model.songs)
            img: utils.thumbnail(model.artist, model.album)
            cindex: index
            contentHeight: Theme.itemSizeMedium

            menu: ContextMenu {
                visible: model.name !== "00000000000000000000"

                /*MenuItem {
                    text: qsTr("Edit metadata")
                    onClicked: {
                        helperList.clear()
                        myplaylistmanager.loadAlbum(model.artist, model.album, "1")
                        meta.setFile(decodeURIComponent(helperList.get(0).url))
                        helperList2.clear()
                        for ( var i=0; i<helperList.count; ++i )
                            helperList2.append(helperList.get(i))
                        pageStack.push ("AlbumMetadata.qml", {"artist":model.artist,
                                            "album":model.album, "artistcount":"1"})
                    }
                }*/
                MenuItem {
                    text: qsTr("Add to playlist")
                    onClicked: {
                        helperList.clear()
                        /*myplaylistmanager.loadAlbum(model.artist, model.album, "1")
                        helperList2.clear()
                        for ( var i=0; i<helperList.count; ++i )
                            helperList2.append(helperList.get(i))*/
                        filterArtist = model.artist
                        filterAlbum = model.album
                        filterArtistCount = "1"
                        filterSong = ""
                        pageStack.push("SelectPlaylist.qml")
                    }
                }
            }

            onClicked: {
                pageStack.push("SongListView.qml", {"artist":model.artist, "album":model.album, "artistcount":"1"})
            }

        }

    }

    Component {
        id: mainColumn

        Column {
            id: column1
            width: parent.width
            //spacing: Theme.paddingMedium

            Item {
                width: parent.width
                height: dataCol.height + Theme.paddingLarge

                Rectangle {
                    anchors.fill: parent
                    color: Theme.highlightBackgroundColor
                    opacity: 0.1
                }

                Column {
                    id: dataCol
                    width: parent.width
                    spacing: Theme.paddingMedium

                    Header {
                        title: artist
                    }

                    Row {
                        spacing: Theme.paddingMedium
                        x: Theme.paddingLarge
                        width: parent.width -Theme.paddingLarge*2
                        height: thumb.height

                        CoverArtList {
                            id: thumb
                            width: Theme.itemSizeExtraLarge //itemimg===""? Theme.itemSizeExtraLarge : itemwidth
                            height: Theme.itemSizeExtraLarge
                            itemimg: utils.thumbnailArtist(root.artist)
                            artist: root.artist
                            album: ""
                            text: qsTr("Image not found")
                            showBorder: true
                        }

                        Column {
                            spacing: Theme.paddingMedium
                            anchors.bottom: parent.bottom
                            Label {
                                font.pixelSize: Theme.fontSizeSmall
                                text: songlist.count==1 ? qsTr("1 album") : qsTr("%1 albums").arg(songlist.count )
                            }

                            Label {
                                font.pixelSize: Theme.fontSizeExtraSmall
                                text: totalSongs==1 ? qsTr("1 track") : qsTr("%1 tracks").arg(totalSongs)
                            }
                        }
                    }

                }
            }
        }


    }

}
