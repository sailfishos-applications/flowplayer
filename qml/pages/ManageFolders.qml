import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Page {
    id: root

    allowedOrientations: appWindow.pagesOrientations

    property bool loaded: false

    onStatusChanged: {
        if (status===PageStatus.Activating && !loaded) {
            foldersModel.clear()
            utils.getFolders()
            loaded = true
        }
    }

    ListModel { id: foldersModel }

    Connections {
        target: utils

        onAddFolder: {
            foldersModel.append({"name":name, "path":path})
        }

        onFoldersChanged: {
            foldersModel.clear()
            utils.getFolders()
        }
    }


    SilicaListView {
        id: flist
        anchors.fill: parent

        model: foldersModel

        PullDownMenu {
            MenuItem {
                text: qsTr("Add folder")
                onClicked: pageStack.push(hasPickers ? "PickFolder.qml" : "AddFolder.qml")
            }
        }

        header: Header {
            title: qsTr("Manage folders")
        }

        delegate: ListItem {
            contentHeight: Theme.itemSizeSmall
            width: parent.width
            clip: true

            menu: contextMenu

            function removeItem() {
                remorseAction(qsTr("Deleting"),
                function() {
                    utils.removeFolder(model.path)
                    foldersModel.remove(model.index, 1)
                })
            }

            Component {
                id: contextMenu
                ContextMenu {
                    MenuItem {
                        text: qsTr("Remove")
                        onClicked: {
                            removeItem()
                        }
                    }
                }
            }

            Image {
                id: licon
                anchors.left: parent.left
                anchors.leftMargin: Theme.paddingLarge
                anchors.verticalCenter: parent.verticalCenter
                source: "image://theme/icon-m-folder"
            }

            Column {
                anchors.left: licon.right
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
                    text: model.path
                    truncationMode: TruncationMode.Fade
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeExtraSmall
                    opacity: 0.8
                }
            }
        }

        ViewPlaceholder {
            enabled: foldersModel.count===0
            flickable: flist
            text: qsTr("No folders")
        }

    }



}
