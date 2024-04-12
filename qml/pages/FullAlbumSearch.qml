import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Page {
    id: root

    allowedOrientations: appWindow.pagesOrientations

    property bool loaded: false

    property bool searchFinished: true
    property bool downloadFinished: false

    property int ok
    function checkAllItems() {
        //console.log("CHECKING ITEMS")
        ok = 1
        for ( var i=0; i<songlist.count; ++i ) {
            songlist.currentIndex = i
            if (songlist.currentItem.loadSel===true) {
                ok = 0
                //console.log("ITEM "+songlist.currentItem.myData.title+" LOADING" )
            } else {
                if (songlist.currentItem.img!=="" && songlist.currentItem.img!=="ERROR")
                   if (!songlist.currentItem.isSel) missing.selectItem(i)
            }
        }
        if (ok==1) {
            //console.log("EVERYTHING LOADED!")
            //selectButton.text = qsTr("Save selected")
            searchFinished = true
            downloadFinished = true
        }
    }

    onStatusChanged: {
        if (status===PageStatus.Active) {
            missing.loadData()
        }
        if (status===PageStatus.Deactivating) {
            missing.clearList()
            missing.cancelAll()
            searchFinished = true
        }
    }

    Connections {
        target: missing

        onLoadChanged: {
            loaded = true
        }
    }


    SilicaFlickable {
        id: flickArea
        anchors.fill: parent
        contentWidth: width
        contentHeight: column.height + Theme.paddingLarge
        clip: true

        PullDownMenu {
            enabled: songlist.count>0

            MenuItem {
                text: qsTr("Start downloading")
                visible: !missing.runthread
                onClicked: {
                    searchFinished = false
                    for ( var i=0; i<songlist.count; ++i ) {
                        songlist.currentIndex = i
                        songlist.currentItem.loadSel = true
                        var art = songlist.currentItem.myData.artist
                        if (songlist.currentItem.myData.artist!==songlist.currentItem.myData.band)
                            art = songlist.currentItem.myData.title
                        missing.addMissing(art, songlist.currentItem.myData.title, i)
                    }
                    missing.startDownload()
                }
            }
            MenuItem {
                text: qsTr("Stop downloading")
                visible: missing.runthread
                onClicked: {
                    missing.cancelAll()
                    searchFinished = true
                    pageStack.pop()
                }
            }
        }

        Column {
            id: column
            width: parent.width
            //spacing: Theme.paddingLarge

            Header {
                title: qsTr("Download album covers")
            }

            SilicaListView {
                id: songlist
                x: 0
                width: parent.width
                interactive: false
                height: songlist.count*Theme.itemSizeMedium
                model: missing
                //cacheBuffer: 10000
                clip: true
                highlightFollowsCurrentItem: false

                delegate: DownloadingListDelegate {
                    myData: model
                    name: model.title
                    desc: model.band
                    img: model.coverart
                    isSel: model.isSelected
                    loadSel: false
                    height: Theme.itemSizeMedium
                    cindex: index
                    onClicked: missing.selectItem(index)
                    onLoadFinished: checkAllItems()
                }

            }

        }

        ViewPlaceholder {
            enabled: songlist.count===0 && loaded
            Behavior on opacity { FadeAnimation {} }
            text: qsTr("There are no missing covers in your music collection")
        }

    }

    BusyIndicator {
        id: busy
        anchors.centerIn: root
        size: BusyIndicatorSize.Large
        visible: !loaded
        running: visible
    }



}
