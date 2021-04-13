import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Page {
    id: root
    property bool pageloaded: false

    allowedOrientations: appWindow.pagesOrientations

    /*onStatusChanged: {
        if (status===PageStatus.Active) {
            if (radioModel.count===0) {
                radios.loadRadios()
            }
        }
    }*/

    /*Connections {
        target: radios

        onAppendRadio: {
            radioModel.append({"name":name, "url":genre, "radioid":radioid, "image":image})
        }
    }*/


    SilicaListView {
        id: songlist
        anchors.fill: parent

        model: radioModel

        PullDownMenu {
            MenuItem {
                text: qsTr("Add station")
                onClicked: pageStack.push("AddRadio.qml")
            }
            MenuItem {
                text: qsTr("Search station")
                onClicked: pageStack.push("SearchRadio.qml")
            }
        }

        header: Header {
            title: qsTr("Radio stations")
        }

        delegate: ListItem {
            contentHeight: Theme.itemSizeSmall
            width: parent.width
            clip: true

            menu: contextMenu

            function removeItem() {
                remorseAction(qsTr("Deleting"),
                function() {
                    radios.removeRadio(model.name, model.url)
                    radioModel.remove(model.index, 1)
                })
            }

            Component {
                id: contextMenu
                ContextMenu {
                    MenuItem {
                        text: qsTr("Edit")
                        onClicked: {
                            pageStack.push("AddRadio.qml", {"editingRadio":true, "previd":model.id,
                                               "prevname":model.name, "prevurl":model.url, "previmage":model.image})
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
                    text: model.name
                    truncationMode: TruncationMode.Fade
                    color: Theme.primaryColor
                }

                Label
                {
                    width: parent.width
                    text: model.url
                    truncationMode: TruncationMode.Fade
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeExtraSmall
                    opacity: 0.8
                }
            }

            onClicked: {
                console.log("Playing radio: " + model.url + " - " + model.radioid )
                playingRadio = true
                queueList.clear()
                currentSongInfo = {name:model.name, url:model.url, imageurl:model.image, radioid:model.radioid, artist:"", album:"", title:""}
                queueList.append({"artist":"", "album":"", "title":"", "name":model.name, "radioid":model.radioid,
                                   "imageurl":model.url, "url":model.image})
                myPlayer.setSource(model.url)
                myPlayer.play()
                miniPlayer.open = true
                utils.removeAlbumArt()
                console.log("Fetching Info..." + model.name)
                radios.getPlayingInfo(model.radioid)

            }


        }

        ViewPlaceholder {
            enabled: radioModel.count===0
            flickable: songlist
            text: qsTr("No saved stations")
        }

    }



}
