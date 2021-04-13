import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Page {
    id: root

    allowedOrientations: appWindow.pagesOrientations

    property bool loaded: false

    onStatusChanged: {
        if (status === PageStatus.Activating) {
            loaded = false
            orientationMenu.currentIndex = savedorientation==="auto"? 0 : (savedorientation==="portrait"? 1 : 2)
            albumsViewMenu.currentIndex = utils.viewmode==="flow"? 0 : utils.viewmode==="grid"? 1 : 2
            albumsSortMenu.currentIndex = utils.order==="album"? 0 : 1
            loaded = true
        }
    }

    Connections {
        target: appWindow

        onSetLanguage: {
            langValue.value = langval
        }
    }


    SilicaFlickable {
        id: flickArea
        anchors.fill: parent
        contentWidth: width
        contentHeight: column.height + Theme.paddingLarge
        clip: true

        PullDownMenu {
            MenuItem {
                text: qsTr("About")
                onClicked: pageStack.push("AboutPage.qml")
            }
        }

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingMedium

            Header {
                title: qsTr("Settings")
            }

            SectionHeader {
                text: qsTr("General")
            }

            Column {
                width: parent.width

                ValueButton {
                    id: langValue
                    label: qsTr("Language")
                    value: utils.readSettings("LanguageName", "English")
                    onClicked: {
                        pageStack.push("SelectLanguage.qml")
                    }
                }

                Label {
                    color: Theme.secondaryColor
                    width: parent.width -Theme.paddingLarge*2
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.paddingLarge
                    wrapMode: Text.Wrap
                    text: qsTr("*Restart to apply the new language")
                    font.pixelSize: Theme.fontSizeExtraSmall
                }
            }

            ComboBox {
                id: orientationMenu
                label: qsTr("Orientation")
                menu: ContextMenu {
                    MenuItem { text: qsTr("Automatic") }
                    MenuItem { text: qsTr("Portrait") }
                    MenuItem { text: qsTr("Landscape") }
                }
                onCurrentItemChanged: {
                    if (!loaded) return
                    if (currentIndex == 0) {
                        utils.setSettings("Orientation", "auto")
                        savedorientation = "auto"
                    }
                    else if (currentIndex == 1) {
                        utils.setSettings("Orientation", "portrait")
                        savedorientation = "portrait"
                    }
                    else {
                        utils.setSettings("Orientation", "landscape")
                        savedorientation = "landscape"
                    }
                }
            }


            ComboBox {
                id: albumsViewMenu
                label: qsTr("Albums view mode")
                menu: ContextMenu {
                    MenuItem { text: qsTr("Flow") }
                    MenuItem { text: qsTr("Grid") }
                    MenuItem { text: qsTr("List") }
                }
                onCurrentItemChanged: {
                    if (!loaded) return
                    if (currentIndex == 0) {
                        utils.setViewMode("flow")
                    }
                    else if (currentIndex == 1) {
                        utils.setViewMode("grid")
                    }
                    else {
                        utils.setViewMode("list")
                    }
                }
            }

            SectionHeader {
                text: qsTr("Active cover")
            }

            TextSwitch {
                text: qsTr("Use album art as background")
                checked: utils.readSettings("BigCoverBackground", "no")==="yes"
                onClicked: {
                    utils.setSettings("BigCoverBackground", checked? "yes" : "no")
                    showBigCover = checked
                }
            }

            TextSwitch {
                text: qsTr("Show previous button")
                checked: utils.readSettings("ShowPreviousButton", "no")==="yes"
                onClicked: {
                    utils.setSettings("ShowPreviousButton", checked? "yes" : "no")
                    showPrevCover = checked
                }
            }


            SectionHeader {
                text: qsTr("List management")
            }

            ComboBox {
                id: albumsSortMenu
                label: qsTr("Sort albums by")
                menu: ContextMenu {
                    MenuItem { text: qsTr("Albums") }
                    MenuItem { text: qsTr("Artists") }
                }
                onCurrentItemChanged: {
                    if (!loaded) return
                    if (currentIndex == 0) {
                        utils.setOrder("album")
                    }
                    else {
                        utils.setOrder("artist")
                    }
                    misdatos.clearList()

                    if (lastGroup=="albums")
                        misdatos.loadAlbums(utils.order)
                    else if (lastGroup=="artists")
                        misdatos.loadArtists()

                }
            }

            ComboBox {
                id: songsSortMenu
                label: qsTr("Sort songs by")
                menu: ContextMenu {
                    MenuItem { text: qsTr("Title") }
                    MenuItem { text: qsTr("Track number") }
                    MenuItem { text: qsTr("Filename") }
                }
                currentIndex: utils.readSettings("TrackOrder", "title")==="title"? 0 :
                              utils.readSettings("TrackOrder", "title")==="number"? 1 : 2
                onCurrentItemChanged: {
                    if (!loaded) return
                    if (currentIndex == 0) {
                        utils.setSettings("TrackOrder", "title")
                    }
                    else if (currentIndex == 1) {
                        utils.setSettings("TrackOrder", "number")
                    }
                    else {
                        utils.setSettings("TrackOrder", "filename")
                    }
                    if (lastGroup==="songs") {
                        misdatos.clearList()
                        misdatos.loadSongs(utils.readSettings("TrackOrder", "title"))
                    }
                }
            }

            TextSwitch {
                text: qsTr("Clear queue on exit")
                checked: utils.cleanqueue==="yes"
                onClicked: {
                    utils.setCleanQueue(checked?"yes":"no")
                }
            }

            SectionHeader {
                text: qsTr("Lyrics")
            }


            TextSwitch {
                text: qsTr("Fetch lyrics automatically")
                checked: utils.autosearch==="yes"
                onClicked: {
                    utils.setAutoSearch(checked?"yes":"no")
                }
            }

            TextSwitch {
                text: qsTr("Save lyrics automatically")
                checked: utils.readSettings("SaveAfterSearch", "no")==="yes"
                onClicked: {
                    utils.setSettings("SaveAfterSearch", checked? "yes" : "no")
                }
            }

            SectionHeader {
                text: qsTr("Extras")
            }

            TextSwitch {
                text: qsTr("Gapless playback")
                checked: utils.readSettings("GaplessPlayback", "no")==="yes"
                onClicked: {
                    utils.setSettings("GaplessPlayback", checked? "yes" : "no")
                    gaplessPlayback = checked
                }
            }

            SectionHeader {
                text: qsTr("Collection")
            }

            ListItem {
                width: parent.width
                height: Theme.itemSizeSmall
                Label {
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.right: licon.left
                    anchors.rightMargin: Theme.paddingLarge
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Manage folders")
                }
                Image {
                    id: licon
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.paddingMedium
                    anchors.verticalCenter: parent.verticalCenter
                    source: "image://theme/icon-m-right"
                }
                onClicked: pageStack.push("ManageFolders.qml")
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Update collection")
                onClicked: {
                    database.readMusic()
                    pageStack.pop()
                }
            }


        }

    }


}
