import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Dialog {
    id: root

    allowedOrientations: appWindow.pagesOrientations

    onAccepted: {
        utils.addFolderToList(currentPath)
        //pageStack.pop()
    }

    onStatusChanged: {
        if (status===PageStatus.Activating) {
            foldersModel.clear()
            utils.getFolderItems(currentPath)
        }
    }

    ListModel { id: foldersModel }

    Connections {
        target: utils

        onAppendFile: {
            foldersModel.append({"name":name, "path":path, "icon":icon})
        }

        onAppendFilesDone: currentPath = path
    }

    property string currentPath

    DialogHeader {
        id: header
        acceptText: qsTr("Done")
        cancelText: qsTr("Cancel")
        spacing: 0
    }

    SilicaFlickable {
        id: flick
        anchors.fill: parent
        anchors.topMargin: header.height
        clip: true

        Header {
            id: header2
            title: qsTr("Select folder")
        }

        IconButton {
            id: upBtn
            enabled: currentPath!=="/"
            anchors.verticalCenter: header2.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingMedium
            icon.source: "image://theme/icon-m-up"
            onClicked: {
                foldersModel.clear()
                utils.getFolderItemsUp(currentPath);
            }
        }

        SilicaListView {
            id: flist
            anchors.fill: parent
            anchors.topMargin: header2.height
            clip: true

            model: foldersModel

            delegate: ListItem {
                contentHeight: Theme.itemSizeSmall
                width: parent.width
                clip: true

                enabled: model.icon==="folder"

                menu: contextMenu

                function removeItem() {
                    remorseAction(qsTr("Deleting"),
                    function() {
                        utils.removeFolder(model.location)
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
                    source: "image://theme/icon-m-" + model.icon
                }

                Label
                {
                    anchors.left: licon.right
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.paddingLarge
                    anchors.verticalCenter: parent.verticalCenter
                    truncationMode: TruncationMode.Fade
                    text: model.name
                }

                onClicked: {
                    var p = model.path
                    foldersModel.clear()
                    utils.getFolderItems(p);
                }

            }

        }

        ViewPlaceholder {
            enabled: foldersModel.count===0
            flickable: flick
            text: qsTr("No items")
        }

    }

}
