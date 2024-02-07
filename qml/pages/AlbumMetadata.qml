import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0


Dialog {
    id: root

    allowedOrientations: appWindow.pagesOrientations

    property string artist
    property string album
    property string artistcount


    onAccepted: {
        //titleField.text = meta.title
        //meta.setData(titleField.text, artistField.text, albumField.text, yearField.text)
        for ( var i=0; i<helperList2.count; ++i )
        {
            meta.setFile(helperList2.get(i).url,"false")
            console.log("TAG: " + artistField.text + " - " + albumField.text + " - " + yearField.text)
            meta.setAlbumData(artistField.text, albumField.text, yearField.text)
        }

        albumMetadataChanged(artist, artistField.text, album, albumField.text, "!album!", "!album!")
        misdatos.clearList()
        misdatos.loadData(utils.order)
    }

    DialogHeader {
        id: header
        acceptText: qsTr("Done")
        cancelText: qsTr("Cancel")
        spacing: 0
    }

    SilicaFlickable {
        id: flicker

        anchors.fill: parent
        anchors.topMargin: header.height
        contentHeight: column.height + Theme.paddingLarge
        clip: true

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingLarge

            Header {
                title: qsTr("Metadata editor")
            }

            CoverArtList {
                id: imgtomask
                x: Theme.paddingLarge
                itemwidth:  Theme.itemSizeExtraLarge
                itemheight: itemwidth
                itemimg: utils.thumbnail(meta.artist, meta.album)
                text: qsTr("Cover not found")
                showBorder: true
            }

            TextField {
                id: artistField
                width: parent.width
                text: meta.artist
                placeholderText: qsTr("Artist")
                label: qsTr("Artist")
                //focus: true
                EnterKey.enabled: artistField.text.length>0
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: albumField.forceActiveFocus()
            }

            TextField {
                id: albumField
                width: parent.width
                text: meta.album
                placeholderText: qsTr("Album")
                label: qsTr("Album")
                //focus: true
                EnterKey.enabled: albumField.text.length>0
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: yearField.forceActiveFocus()
            }

            TextField {
                id: yearField
                width: parent.width
                text: meta.year
                placeholderText: qsTr("Year")
                label: qsTr("Year")
                //focus: true
                EnterKey.enabled: yearField.text.length>0
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                inputMethodHints: Qt.ImhDigitsOnly
                //EnterKey.onClicked: dialog.accept()
            }

        }

    }


}
