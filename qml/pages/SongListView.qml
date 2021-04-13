import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0
import "dateandtime.js" as DT

Page {
    id: root

    allowedOrientations: appWindow.pagesOrientations

    property string artist
    property string album
    property string artistcount

    property string totalTime: ""

    property bool loaded: false
    property bool isqueue: false

    ListModel { id: songListModel }

    onStatusChanged: {
        if (status===PageStatus.Activating && !loaded) {
            console.log("Loading list: " + artist + " - " + album + " - " + artistcount)
            //musicmodel.clearList()
            //musicmodel.loadData(artist, album, artistcount)
            songListModel.clear()
            myplaylistmanager.loadAlbum(artist, album, artistcount)
            loaded = true
            isqueue = false
        }
    }

    Connections {
        target: appWindow

        onAlbumMetadataChanged: {
            if (artist===artistold && album===albumold) {
                artist = artistnew
                album = albumnew
                //musicmodel.clearList()
                //musicmodel.loadData(artist, album, artistcount)
                songListModel.clear()
                myplaylistmanager.loadAlbum(artist, album, artistcount)
            }
        }
    }

    Connections {
        target: myplaylistmanager

        onAddItemToAlbum: {
            songListModel.append({"index":songListModel.count, "artist":item.artist, "album":item.album, "title":item.title,
                               "duration":item.duration, "url":item.url, "fav":item.fav})
        }

        onAlbumLoaded: {
            console.log("Album loaded. Time: " + totaltime)
            totalTime = DT.getDuration(totaltime)
        }

    }

    SilicaListView {
        id: songlist
        anchors.fill: parent
        clip: true

        PullDownMenu {
            MenuItem {
                text: qsTr("Search cover")
                enabled: artist!==qsTr("Unknown artist") && album!==qsTr("Unknown album")
                onClicked: {
                    coversearch.clearData()
                    pageStack.push ("CoverDownload.qml", {"artist":(artistcount==="1"?artist:album), "album":album, "artistcount":artistcount})
                }
            }
            MenuItem {
                text: qsTr("Edit metadata")
                onClicked: {
                    meta.setFile(decodeURIComponent(songListModel.get(0).url))
                    helperList2.clear()
                    for ( var i=0; i<songListModel.count; ++i )
                        helperList2.append(songListModel.get(i))
                    pageStack.push ("AlbumMetadata.qml", {"artist":artist, "album":album, "artistcount":artistcount})
                }
            }
            MenuItem {
                text: qsTr("Add to playlist")
                onClicked: {
                    /*helperList2.clear()
                    for ( var i=0; i<songListModel.count; ++i )
                        helperList2.append(songListModel.get(i))*/
                    filterArtist = root.artist
                    filterAlbum = root.album
                    filterSong = ""
                    pageStack.push("SelectPlaylist.qml")
                }
            }
        }

        model: songListModel

        header: mainColumn

        delegate: AlbumDelegate {
            myData: model
            name: model.title
            desc: model.artist
            img: ""
            time: DT.getDuration(model.duration)
            cindex: index
            isplaying: decodeURIComponent(model.url) === decodeURIComponent(myPlayer.source)
            contentHeight: Theme.itemSizeSmall

            menu: ContextMenu {
                visible: model.name !== "00000000000000000000"

                MenuItem {
                    text: qsTr("Edit metadata")
                    onClicked: {
                        meta.setFile(decodeURIComponent(model.url))
                        pageStack.push ("Metadata.qml", {"artist": model.artist,
                                            "album": model.album,
                                            "title": model.title
                                        })
                    }
                }
                MenuItem {
                    text: qsTr("Add to playlist")
                    onClicked: {
                        filterArtist = ""
                        filterAlbum = ""
                        filterSong = model.url
                        pageStack.push("SelectPlaylist.qml")
                    }
                }
                MenuItem {
                    text: model.fav==="1"? qsTr("Remove from favorites") : qsTr("Add to favorites")
                    onClicked: {
                        if (model.fav==="1") {
                            utils.favSong(model.url, false)
                            songListModel.setProperty(model.index, "fav", "")
                            favRemoved()
                        } else {
                            utils.favSong(model.url, true)
                            songListModel.setProperty(model.index, "fav", "1")
                            favAdded()
                        }
                    }
                }
            }

            onClicked: {
                if (isqueue)
                {
                    nowPlayingPage.playSong(model.index)
                }
                else
                {
                    var cindex = model.index
                    queueList.clear()
                    myplaylistmanager.clearList("00000000000000000000")
                    myplaylistmanager.addAlbumToList("00000000000000000000", root.artist, root.album, root.artistcount, "")
                    myplaylistmanager.loadPlaylist("00000000000000000000")
                    queueList.currentIndex = cindex
                    nowPlayingPage.playSong(cindex)
                    isqueue = true
                }
                miniPlayer.open = true
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
                        title: album
                    }

                    Row {
                        spacing: Theme.paddingMedium
                        x: Theme.paddingLarge
                        width: parent.width -Theme.paddingLarge*2

                        CoverArtList {
                            id: thumb
                            width: Theme.itemSizeExtraLarge
                            height: width
                            itemimg: utils.thumbnail(root.artist, root.album, root.artistcount)
                            artist: root.artist
                            album: root.album
                            text: qsTr("Cover not found")
                            showBorder: true
                        }

                        Column {
                            spacing: Theme.paddingMedium
                            Label {
                                font.pixelSize: Theme.fontSizeMedium
                                text: artistcount==="1" ? artist : qsTr("Various artists")
                            }
                            Label {
                                font.pixelSize: Theme.fontSizeSmall
                                text: songlist.count==1 ? qsTr("1 track") : qsTr("%1 tracks").arg(songlist.count)
                            }
                            Label {
                                id: tcount
                                font.pixelSize: Theme.fontSizeExtraSmall
                                text: totalTime
                            }
                        }
                    }

                }
            }

        }

    }

}
