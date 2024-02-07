import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0
import "dateandtime.js" as DT

Page {
    id: root

    allowedOrientations: appWindow.pagesOrientations

    property bool loaded: false
    property bool isqueue: false

    property bool showSearch: false
    property string searchValue: ""
    property bool enableSearch: false

    function searchData(text) {
        searchValue = text
        misdatos.setFilter(searchValue)
        enableSearch = false
        isqueue = false
    }

    SilicaListView {
        id: songlist
        anchors.fill: parent
        clip: true

        PullDownMenu {
            MenuItem {
                text: showSearch? qsTr("Hide search field") : qsTr("Show search field")
                onClicked: {
                    if (searchValue!="") {
                        searchValue = ""
                        misdatos.setFilter(searchValue)
                    }
                    showSearch = !showSearch
                    enableSearch = false
                }
            }
        }

        model: misdatos

        header: Item {
            height: head2.height + search2.height
            width: parent.width

            Header {
                id: head2
                title: qsTr("Tracks")
            }

            Item {
                id: search2
                width: parent.width
                anchors.top: head2.bottom
                height: showSearch? searchInput2.height : 0
                Behavior on height { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad; property: "height" } }
                onHeightChanged: songlist.positionViewAtBeginning()
                clip: true

                TextField {
                    id: searchInput2
                    anchors.bottom: parent.bottom
                    width: parent.width - sbutton2.width
                    //placeholderText: qsTr("Search")
                    inputMethodHints: Qt.ImhNoPredictiveText
                    //enabled: showSearch
                    //onEnabledChanged: text = ""
                    //focus: enabled
                    //visible: opacity > 0
                    opacity: showSearch ? 1 : 0
                    Behavior on opacity { FadeAnimation { duration: 200 } }
                    onTextChanged: enableSearch = searchInput2.text!==searchValue
                    EnterKey.enabled: enableSearch && searchInput2.text!==""
                    EnterKey.onClicked: searchData(searchInput2.text)
                }

                Connections {
                    target: misdatos
                    onLoadChanged: searchInput2.text = searchValue
                }

                IconButton {
                    id: sbutton2
                    smooth: true
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.paddingMedium
                    height: Theme.itemSizeExtraSmall
                    width: height
                    icon.source: enableSearch && searchInput2.text!==""? "image://theme/icon-m-search" : "image://theme/icon-m-cancel"
                    icon.height: height -Theme.paddingMedium
                    icon.width: icon.height
                    enabled: searchInput2.text!=="" || searchValue!==""
                    onClicked: {
                        if (!enableSearch)
                            searchInput2.text = ""
                        searchData(searchInput2.text)
                    }
                }
            }
        }

        delegate: AlbumDelegate {
            myData: model
            name: replaceText(model.title, searchValue)
            desc: replaceText(model.artist + " - " + model.album, searchValue)
            img: ""
            time: DT.getDuration(model.duration)
            cindex: index
            isplaying: decodeURIComponent(model.url) === decodeURIComponent(myPlayer.source)
            contentHeight: Theme.itemSizeSmall

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Edit metadata")
                    onClicked: {
                        console.log("METADATA: " + model.url)
                        meta.setFile(model.url)
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
                        if (model.fav==="1")
                            favRemoved()
                        else
                            favAdded()
                        misdatos.setFav(model.url, model.fav)
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

                    if (searchValue==="")
                    {
                        myplaylistmanager.addAlbumToList("00000000000000000000", "ALL", "ALL", "ALL", "")
                    }
                    else
                    {
                        misdatos.addFilterToQueue()
                    }
                    myplaylistmanager.loadPlaylist("00000000000000000000")
                    queueList.currentIndex = cindex
                    nowPlayingPage.playSong(cindex)
                    isqueue = true
                }
                miniPlayer.open = true
            }

        }

        ViewPlaceholder {
            enabled: songlist.count===0
            flickable: songlist
            text: qsTr("No music")
        }
    }


}
