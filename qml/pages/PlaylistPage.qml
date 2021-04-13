import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0
import "dateandtime.js" as DT


Page {
    id: root

    allowedOrientations: appWindow.pagesOrientations

    property bool loaded: false
    property bool isqueue: false

    ListModel { id: playlistModel }

    onStatusChanged: {
        if (status===PageStatus.Activating && !loaded) {
            console.log("current list: " + currentPlaylist + " - clean queue: " + utils.cleanqueue)

            loaded = true
            if (currentPlaylist==="00000000000000000000" && queueList.count>0)
                return;

            playlistModel.clear()
            myplaylistmanager.loadPlaylist(currentPlaylist)
        }
    }

    Connections {
        target: appWindow

        onAlbumMetadataChanged: {
            playlistModel.clear()
            myplaylistmanager.loadPlaylist(currentPlaylist)
        }
    }

    Connections {
        target: myplaylistmanager

        onAddItemToList: {
            if (currentPlaylist===list && currentPlaylist!=="00000000000000000000") {
                console.log("Adding to " + currentPlaylist + ": " + item.title)
                playlistModel.append({"artist":item.artist, "album":item.album, "title":item.title,
                                   "duration":item.duration, "url":item.url})
            }
        }
    }


    SilicaListView {
        id: songlist
        anchors.fill: parent
        highlightFollowsCurrentItem: false
        //clip: true

        header: Header {
            title: currentPlaylist==="00000000000000000000" ? qsTr("Queue") :
                   currentPlaylist==="00000000000000000001" ? qsTr("Favorites") : currentPlaylist
        }

        PullDownMenu {
            MenuItem {
                visible: currentPlaylist!=="00000000000000000000" && currentPlaylist!=="00000000000000000001"
                text: qsTr("Rename playlist")
                onClicked: {
                    pageStack.push("NewPlaylist.qml", {"renamingPlaylist":true})
                }
            }
            MenuItem {
                text: qsTr("Clear playlist")
                onClicked: {
                    myplaylistmanager.clearList(currentPlaylist)
                    myPlaylists.clear()
                    playlistModel.clear()
                    myplaylistmanager.loadPlaylists()

                    if (currentPlaylist==="00000000000000000000") {
                        queueList.clear()
                        player.stop()
                        player.source = ""
                        closeMiniplayer()
                    }
                }
            }
        }

        model: currentPlaylist==="00000000000000000000"? queueList : playlistModel

        delegate: AlbumDelegate {
            myData: model
            name: model.title
            desc: model.artist
            img: utils.thumbnail(model.artist, model.album)
            time: DT.getDuration(model.duration)
            cindex: index
            isplaying: decodeURIComponent(model.url) === decodeURIComponent(myPlayer.source)
            contentHeight: Theme.itemSizeSmall
            textSize: Theme.fontSizeExtraSmall*0.5
            showCover: true

            function removeItem() {
                remorseAction(qsTr("Deleting"),
                function() {
                    if (currentPlaylist==="00000000000000000000")
                    {
                        console.log("Item is in queue")
                        for ( var i=0; i<queueList.count; ++i )
                        {
                            if (queueList.get(i).url === model.url) {
                                console.log("Removing from queue")
                                queueList.remove(i)
                                break;
                            }
                        }
                        if (queueList.count===0) {
                            player.stop()
                            player.source = ""
                        }
                    }
                    if (currentPlaylist==="00000000000000000001")
                    {
                        utils.favSong(model.url, false)
                        favRemoved()
                    }
                    else
                    {
                        myplaylistmanager.loadList(currentPlaylist)
                        myplaylistmanager.removeFromList(model.url)
                        myplaylistmanager.saveList(currentPlaylist)
                    }
                    playlistModel.remove(model.index)
                })
            }

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Remove")
                    onClicked: {
                        removeItem()
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
                    if (currentPlaylist!=="00000000000000000000") {
                        queueList.clear()
                        //myplaylistmanager.clearList("00000000000000000000")
                        myplaylistmanager.copyListToQueue(currentPlaylist)
                        myplaylistmanager.loadPlaylist("00000000000000000000")
                    }
                    myPlaylists.clear()
                    myplaylistmanager.loadPlaylists()
                    queueList.currentIndex = cindex
                    nowPlayingPage.playSong(cindex)
                    miniPlayer.open = true
                    isqueue = true
                }
            }

        }

        ViewPlaceholder {
            enabled: songlist.count==0
            flickable: songlist
            text: qsTr("Playlist is empty")
        }
    }

}
