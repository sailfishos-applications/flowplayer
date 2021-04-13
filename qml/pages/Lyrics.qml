import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0


Page {
    id: root

    allowedOrientations: appWindow.pagesOrientations

    property bool fetchingLyrics: false
    property bool fetchingInfo: false
    property string currentLyrics: ""

    property bool editmode: false
    property string editArtist
    property string editTitle
    property string editLyrics

    function clearLyrics() {
        currentLyrics = ""
    }

    function reloadLyrics() {
        currentLyrics = ""
        utils.readLyrics(currentSongInfo.artist, currentSongInfo.title)
    }

    function reloadInfo() {
        fetchingInfo = true
        lfm.getBio(currentSongInfo.artist)
        thumb.source = lfm.offlineBioImg(currentSongInfo.artist)
        bioText.text = ""
        bioText.text = lfm.bioInfo()
    }

    Connections {
        target: lfm

        onBioChanged: {
            fetchingInfo = false

            if (thumb.source == "")
                thumb.source = lfm.bioImg()

            bioText.text = ""
            bioText.text = "<style type='text/css'>a:link {color:" + Theme.highlightColor + "; text-decoration:none} " +
                    "a:visited {color:" + Theme.secondaryHighlightColor + "; text-decoration:none}</style>" + lfm.bioInfoLarge()
        }
    }

    Connections {
        target: utils

        onLyricsChanged: {
            currentLyrics = utils.lyrics
            fetchingLyrics = false
            if (!utils.lyricsonline && utils.nolyrics)
            {
                if (utils.autosearch!=="yes")
                    return;

                console.log("Lyrics not found in disk")
                fetchingLyrics = true
                utils.getLyrics(currentSongInfo.artist, currentSongInfo.title, searchServer)
            }
            if (utils.lyricsonline && !utils.nolyrics && utils.readSettings("SaveAfterSearch", "no")==="yes")
            {
                console.log("Saving lyrics automatically")
                utils.saveLyrics(currentSongInfo.artist, currentSongInfo.title, currentLyrics)
            }

        }
    }

    property string searchServer: utils.readSettings("SearchServer", "0")

    onStatusChanged: {
        if (status===PageStatus.Activating) {
            searchServer = utils.readSettings("SearchServer", "0")
            nppOpened = true
        }
    }

    BusyIndicator {
        id: busyIndicator
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        visible: fetchingLyrics && flickArea.visible
        running: visible
    }

    property bool showLyrics: true

    SilicaFlickable {
        id: flickArea
        anchors.fill: parent
        contentHeight: column.height + Theme.paddingLarge
        opacity: showLyrics? 1 : 0
        visible: opacity >0
        Behavior on opacity { FadeAnimation {} }
        clip: true

        PullDownMenu {
            MenuItem {
                text: qsTr("Search in ChartLyrics")
                onClicked: {
                    searchServer = "0"
                    utils.setSettings("SearchServer", searchServer)
                    fetchingLyrics = true
                    utils.getLyrics(currentSongInfo.artist, currentSongInfo.title, searchServer)
                }
            }
            MenuItem {
                text: qsTr("Search in A-Z Lyrics")
                onClicked: {
                    searchServer = "1"
                    utils.setSettings("SearchServer", searchServer)
                    fetchingLyrics = true
                    utils.getLyrics(currentSongInfo.artist, currentSongInfo.title, searchServer)
                }
            }
            MenuItem {
                text: qsTr("Search in Lyric Wiki")
                onClicked: {
                    searchServer = "2"
                    utils.setSettings("SearchServer", searchServer)
                    fetchingLyrics = true
                    utils.getLyrics(currentSongInfo.artist, currentSongInfo.title, searchServer)
                }
            }
            MenuItem {
                text: qsTr("Save lyrics")
                visible: !utils.nolyrics && utils.lyricsonline
                onClicked: {
                    utils.saveLyrics(currentSongInfo.artist, currentSongInfo.title, currentLyrics)
                }
            }
        }



        Column {
            id: column
            x: Theme.paddingLarge
            spacing: Theme.paddingMedium
            width: parent.width - Theme.paddingLarge*2

            Item {
                height: lheader.height
                width: parent.width + Theme.paddingLarge

                Header {
                    id: lheader
                    title: qsTr("Lyrics")
                    width: parent.width -lBtn.width
                }

                IconButton {
                    id: lBtn
                    icon.source: "image://theme/icon-m-about"
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.paddingSmall
                    onClicked: showLyrics = !showLyrics
                }

            }

            Label {
                id: lTextT
                wrapMode: TextEdit.WordWrap
                textFormat: Text.RichText
                width: parent.width
                text: currentSongInfo!==[]? (editmode ? editTitle : currentSongInfo.title) : ""
                color: Theme.highlightColor
            }

            Label {
                id: lTextA
                wrapMode: TextEdit.WordWrap
                textFormat: Text.RichText
                width: parent.width
                font.pixelSize: Theme.fontSizeSmall
                text: currentSongInfo!==[]? (qsTr("by") + " <font color=\"" + Theme.primaryColor + "\">" +
                      (editmode? editArtist : currentSongInfo.artist ) + "</font>") : ""
                color: Theme.secondaryColor
            }

            Label {
                id: lText
                x: Theme.laddingLarge
                visible: !fetchingLyrics && !editmode && currentSongInfo.artist!==[]
                wrapMode: TextEdit.WordWrap
                textFormat: Text.RichText
                width: parent.width - Theme.paddingLarge*2
                font.pixelSize: Theme.fontSizeSmall
                //horizontalAlignment: Text.AlignHCenter
                text: currentLyrics
                color: Theme.secondaryColor
            }
        }

    }

    SilicaFlickable {
        id: infoArea
        anchors.fill: parent
        contentHeight: column2.height + Theme.paddingLarge
        visible: opacity >0
        opacity: showLyrics? 0 : 1
        Behavior on opacity { FadeAnimation {} }
        clip: true

        PullDownMenu {
            MenuItem {
                text: qsTr("Reload picture")
                onClicked: {
                    lfm.removeImage(currentSongInfo.artist)
                    reloadInfo()
                }
            }
            MenuItem {
                text: qsTr("Reload info")
                onClicked: {
                    reloadInfo()
                }
            }
        }

        Column {
            id: column2
            x: Theme.paddingLarge
            spacing: Theme.paddingMedium
            width: parent.width - Theme.paddingLarge*2

            property real imageWidth

            Item {
                height: iheader.height
                width: parent.width + Theme.paddingLarge

                Header {
                    id: iheader
                    title: qsTr("Info")
                    width: parent.width -iBtn.width
                }

                IconButton {
                    id: iBtn
                    icon.source: "image://theme/icon-m-sounds"
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.paddingSmall
                    onClicked: showLyrics = !showLyrics
                }

            }

            Image {
                id: thumb
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                fillMode: Image.PreserveAspectFit
                //visible: false
                //onWidthChanged: column2.imageWidth = parent.width / thumb.width
                cache: false
                smooth: true
            }

            Label {
                id: bioText
                anchors.left: parent.left
                wrapMode: TextEdit.WordWrap
                textFormat: Text.RichText
                width: parent.width
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                onLinkActivated: Qt.openUrlExternally(link)
            }

            BusyIndicator {
                id: busyIndicator2
                size: BusyIndicatorSize.Medium
                anchors.horizontalCenter: parent.horizontalCenter
                visible: fetchingInfo
                running: visible
            }

        }

    }


}
