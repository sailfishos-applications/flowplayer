import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Dialog {

    id: dialog

    property string selectedList

    canAccept: selectedList!==""

    onAccepted: {
        myplaylistmanager.addAlbumToList(selectedList, filterArtist, filterAlbum, filterArtistCount, filterSong)

        if (selectedList==="00000000000000000000") {
            queueList.clear()
            myplaylistmanager.loadPlaylist("00000000000000000000")
        }
    }

    onStatusChanged: {
        if (status===PageStatus.Activating && myPlaylists.count==0)
            myplaylistmanager.loadPlaylists()
    }

    DialogHeader {
        id: header
        acceptText: qsTr("Done")
        cancelText: qsTr("Cancel")
        spacing: 0
    }

    SilicaFlickable {
        anchors.fill: parent
        anchors.topMargin: header.height
        contentHeight: col1.height

        Column {
            id: col1
            width: parent.width

            Header {
                title: qsTr("Select playlist")
            }

            ListItem {
                id: newListItem

                height: Theme.itemSizeSmall
                width: parent.width

                Image {
                    id: icon
                    source: "image://theme/icon-m-add"
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.paddingLarge
                    height: parent.height - Theme.paddingLarge
                    width: height
                }

                Label
                {
                    text: qsTr("New playlist")
                    anchors.left: icon.right
                    anchors.leftMargin: Theme.paddingMedium
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.paddingLarge
                    anchors.verticalCenter: parent.verticalCenter
                    //color: Theme.highlightColor
                }

                onClicked: pageStack.replace("NewPlaylist.qml", {"newListType":"ALBUM"})

            }

            SilicaListView {
                id: songlist
                interactive: false
                width: parent.width
                height: count *Theme.itemSizeSmall

                model: myPlaylists

                delegate: ListItem {
                    visible: name!=="00000000000000000001"
                    contentHeight: visible? Theme.itemSizeSmall : 0
                    width: parent.width
                    clip: true
                    highlighted: down || selectedList==name

                    Label
                    {
                        text: name=="00000000000000000000"? qsTr("Queue") : name
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.paddingLarge
                        anchors.right: parent.right
                        anchors.rightMargin: Theme.paddingLarge
                        anchors.verticalCenter: parent.verticalCenter
                        elide: Text.ElideRight
                        color: name=="00000000000000000000" ? Theme.highlightColor : Theme.primaryColor
                    }

                    onClicked: {
                        selectedList = name
                    }


                }

            }

        }
    }





}
