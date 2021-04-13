import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Page {
    id: foldersPage

    allowedOrientations: appWindow.pagesOrientations

    SilicaListView {
        id: songlist
        anchors.fill: parent

        model: myPlaylists

        PullDownMenu {
            MenuItem {
                text: qsTr("New playlist")
                onClicked: pageStack.push("NewPlaylist.qml", {"newListType":"NOTHING"})
            }
        }

        header: Header {
            title: qsTr("Playlists")
        }

        section.property: "type"
        section.criteria: ViewSection.FullString
        section.delegate: Item {
            visible: section!==""
            height: Theme.itemSizeSmall/2
            width: parent.width
            Separator {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: Theme.paddingLarge
                width: parent.width -Theme.paddingLarge*2
                color: "white"
            }
        }


        delegate: ListItem {
            contentHeight: Theme.itemSizeSmall
            width: parent.width
            clip: true

            menu: model.name==="00000000000000000000" || model.name==="00000000000000000001"? undefined : contextMenu

            function removeItem() {
                remorseAction(qsTr("Deleting"),
                function() {
                    myplaylistmanager.removeList(model.name)
                    myPlaylists.clear()
                    myplaylistmanager.loadPlaylists()
                })
            }

            Component {
                id: contextMenu
                ContextMenu {
                    visible: model.name!=="00000000000000000000" && model.name!=="00000000000000000001"

                    MenuItem {
                        text: qsTr("Rename")
                        onClicked: {
                            currentPlaylist = model.name
                            pageStack.push("NewPlaylist.qml", {"renamingPlaylist":true})
                        }
                    }
                    MenuItem {
                        text: qsTr("Remove")
                        onClicked: {
                            removeItem()
                        }
                    }
                }
            }

            Column {
                anchors.left: parent.left
                anchors.leftMargin: Theme.paddingLarge
                anchors.right: parent.right
                anchors.rightMargin: Theme.paddingLarge
                anchors.verticalCenter: parent.verticalCenter

                Label
                {
                    width: parent.width
                    text: model.name==="00000000000000000000"? qsTr("Queue") :
                          model.name==="00000000000000000001"? qsTr("Favorites") : model.name
                    truncationMode: TruncationMode.Fade
                    color: model.name==="00000000000000000000" ? Theme.highlightColor : Theme.primaryColor
                }

                Label
                {
                    width: parent.width
                    text: model.count==="0"? qsTr("No tracks") : model.count==="1"?
                                                 qsTr("1 track") : qsTr("%1 tracks").arg(model.count)
                    truncationMode: TruncationMode.Fade
                    color: name=="00000000000000000000" ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeExtraSmall
                    opacity: 0.8
                }
            }


            onClicked: {
                currentPlaylist = model.name
                //pageStack.replaceAbove(mainPage, "PlaylistPage.qml")
                pageStack.push("PlaylistPage.qml")
            }


        }

    }

}
