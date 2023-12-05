import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Page {
    id: root
    property bool pageloaded: false

    allowedOrientations: appWindow.pagesOrientations

    property string searchValue: ""
    property bool enableSearch: false

    property bool loading: false

    property string selectedRadio: ""
    property string selectedRadioId: ""

    ListModel { id: searchModel }

    /*onStatusChanged: {
        if (status===PageStatus.Active) {
            if (!pageloaded) {
                radios.searchRadio("vorterix")


            }
        }
    }*/

    Connections {
        target: radios

        onAppendRadioSearch: {
            searchModel.append({"name":name, "genre":genre, "radioid":radioid})
        }

        onAppendRadioDone: loading = false

        onRadioInfoLoaded: {
            loading = false
            if (radiourl==="ERROR") {
                ibanner.displayMessage("Error loading radio", false)
                return;
            }

            console.log("Playing radio: " + radiourl + " - " + selectedRadioId)
            playingRadio = true
            queueList.clear()
            currentSongInfo = {name:selectedRadio, url:radiourl, imageurl:image, radioid:selectedRadioId, artist:"", album:"", title:""}
            queueList.append({"artist":"", "title":"", "album":"", "name":selectedRadio, "radioid":selectedRadioId,
                               "imageurl":image, "url":radiourl})
            myPlayer.setSource(radiourl)
            myPlayer.play()
            miniPlayer.open = true
            utils.removeAlbumArt()
            console.log("Fetching Info..." + selectedRadio)
            radios.getPlayingInfo(selectedRadioId)

        }

    }

    SilicaListView {
        id: songlist
        anchors.fill: parent

        model: searchModel

        header: Item {
            height: head2.height + search2.height
            width: parent.width

            Header {
                id: head2
                title: qsTr("Search station")
            }

            BusyIndicator {
                anchors.left: parent.left
                anchors.leftMargin: Theme.paddingLarge
                anchors.verticalCenter: head2.verticalCenter
                size: BusyIndicatorSize.Small
                visible: loading
                running: visible
            }

            Item {
                id: search2
                width: parent.width
                anchors.top: head2.bottom
                height: searchInput2.height
                Behavior on height { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad; property: "height" } }
                //onHeightChanged: listView.positionViewAtBeginning()
                clip: true

                TextField {
                    id: searchInput2
                    anchors.bottom: parent.bottom
                    width: parent.width - sbutton2.width
                    Behavior on opacity { FadeAnimation { duration: 200 } }
                    onTextChanged: enableSearch = searchInput2.text!==searchValue
                    EnterKey.enabled: searchInput2.text!=="" && !loading
                    EnterKey.onClicked: {
                        searchValue = searchInput2.text
                        enableSearch = false
                        loading = true
                        searchModel.clear()
                        radios.searchRadio(searchInput2.text)
                    }
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
                        if (!enableSearch) {
                            searchInput2.text = ""
                        } else {
                            searchValue = searchInput2.text
                            enableSearch = false
                            loading = true
                            searchModel.clear()
                            radios.searchRadio(searchInput2.text)
                        }
                    }
                }
            }
        }

        delegate: ListItem {
            enabled: !loading
            contentHeight: Theme.itemSizeSmall
            width: parent.width
            clip: true

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
                    text: model.genre
                    truncationMode: TruncationMode.Fade
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeExtraSmall
                    opacity: 0.8
                }
            }

            onClicked: {
                currentPlaylist = model.name
                loading = true
                selectedRadio = model.name
                selectedRadioId = model.radioid
                radios.getRadioInfo(model.radioid)
                //pageStack.push("PlaylistPage.qml")
            }

        }

    }



}
