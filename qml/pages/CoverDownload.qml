import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Page {
    id: clipboardPage

    allowedOrientations: appWindow.pagesOrientations

    property string artist
    property string album
    property string artistcount
    property bool searchingArtist: false
    property bool searching: false

    Connections {
        target: coversearch

        onSearchFinished: {
            view.visible = true
        }

        onDownloadFinished: {
            view.visible = true
            searching = false
        }
    }

    onStatusChanged: {
        if (status===PageStatus.Deactivating)
            coversearch.stopSearch()
    }


    Connections {
        target: utils
        onCoverSaved: {
            pageStack.pop()
        }
    }

    BusyIndicator {
        id: busy
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        visible: !view.visible
        running: visible
    }

    SilicaFlickable {
        id: flicker
        anchors.fill: parent
        contentHeight: column.height
        clip: true

        Column {
            id: column
            width: parent.width

            Header {
                title: searchingArtist? qsTr("Download artist image") : qsTr("Download album cover")
            }


            Item {
                anchors.left: parent.left
                width: parent.width - Theme.paddingMedium
                height: searchField.height

                TextField {
                    id: searchField
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.paddingMedium
                    width: parent.width -Theme.paddingMedium*3 -sbutton.width
                    text: artist + (searchingArtist? "" : " - " + album)
                    placeholderText: qsTr("Enter text to search")
                    labelVisible: false
                }

                IconButton {
                    id: sbutton
                    smooth: true
                    anchors.verticalCenter: searchField.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.paddingMedium
                    height: Theme.itemSizeExtraSmall
                    width: height
                    opacity: !searching? 1 : 0.5
                    icon.source: searching? "image://theme/icon-l-cancel" : "image://theme/icon-m-search"
                    icon.height: height -Theme.paddingMedium
                    icon.width: icon.height
                    onClicked: {
                        if (!searching)
                        {
                            searching = true
                            view.visible = false
                            coversearch.searchCover(artist, album, searchField.text)
                        }
                        else
                        {
                            searching = false
                            view.visible = true
                            coversearch.stopSearch()
                        }
                    }
                }

            }

            SilicaGridView {
                id: view
                interactive: false
                width: parent.width
                visible: true
                cellWidth: appWindow._screenWidth<appWindow._screenHeight? appWindow._screenWidth/3 : appWindow._screenWidth/5
                cellHeight: cellWidth
                cacheBuffer: 3000
                model: coversearch
                height: contentHeight

                delegate: BackgroundItem {
                    id: citem
                    height: view.cellHeight
                    width: view.cellWidth

                    Rectangle
                    {
                        id: mainCover
                        anchors.fill: parent
                        anchors.margins: 1
                        color: Qt.darker(Theme.highlightColor, 2.5)
                        opacity: 0.5
                    }

                    BusyIndicator {
                        id: busy2
                        size: BusyIndicatorSize.Medium
                        anchors.centerIn: parent
                        visible: model.art==="loading"
                        running: visible
                    }

                    Image  {
                        id: urlImage
                        source: "file://" + model.art
                        asynchronous: true
                        anchors.fill: parent
                        fillMode: Image.Stretch
                        //cache: false
                    }

                    onClicked: {
                        if (model.art!=="loading")
                        {
                            console.log("Selecting image: " + model.art)
                            if (searchingArtist)
                                coversearch.saveArtistImage(artist, model.art)
                            else
                                coversearch.saveImage(artist, album, model.art)
                            pageStack.pop()
                        }
                    }

                }
            }

        }
    }

 }

