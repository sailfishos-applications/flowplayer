import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Page {
    id: dialog

    allowedOrientations: appWindow.pagesOrientations

    onStatusChanged: {
        if (status === PageStatus.Activating) {
            presetsModel.clear()
            selValue = utils.readSettings("Preset", "Rock")
            utils.loadPresets()
        }
    }

    property string prevValue
    property string selValue: "Rock"

    function selectPreset(val) {
        selValue = val
        utils.setSettings("Preset", selValue)
        myPlayer.loadEqualizer()
        myPlayer.applyEqualizer()
    }

    ListModel {
        id: presetsModel
    }

    Connections {
        target: utils
        onAppendPreset: presetsModel.append({"name": name})
    }

    Connections {
        target: myPlayer
        onPresetSaved: {
            presetsModel.clear()
            utils.loadPresets()
        }
    }


    SilicaListView {
        id: songlist
        anchors.fill: parent

        model: presetsModel

        header: Header {
            title: qsTr("Presets")
        }

        delegate: ListItem {
            contentHeight: Theme.itemSizeSmall
            width: parent.width
            clip: true

            highlighted: down || model.name===selValue

            menu: contextMenu

            function removeItem() {
                remorseAction(qsTr("Deleting"),
                function() {
                    utils.removePreset(model.name)
                    presetsModel.remove(model.index, 1)
                })
            }

            Component {
                id: contextMenu
                ContextMenu {
                    MenuItem {
                        text: qsTr("Edit")
                        onClicked: pageStack.push("EditPreset.qml", {"editingPreset":true,"prevname":model.name})
                    }
                    MenuItem {
                        text: qsTr("Remove")
                        onClicked: removeItem()
                    }
                }
            }

            Label {
                anchors.left: parent.left
                anchors.leftMargin: Theme.paddingLarge
                anchors.right: parent.right
                anchors.rightMargin: Theme.paddingLarge
                anchors.verticalCenter: parent.verticalCenter
                text: model.name
                truncationMode: TruncationMode.Fade
            }

            onClicked: selectPreset(model.name)
        }

    }

}
