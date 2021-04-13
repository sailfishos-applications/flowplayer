import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Dialog {
    id: root

    allowedOrientations: appWindow.pagesOrientations

    property string artist
    property string album
    property string title
    property string artistcount

    onAccepted: {
        //titleField.text = meta.title


        if (artistField.visible) {
            meta.setData(titleField.text, artistField.text, albumField.text, yearField.text)
            albumMetadataChanged(artist, artistField.text, album, albumField.text, title, titleField.text)
        } else {
            meta.setData(titleField.text, artist, album, 0)
            albumMetadataChanged(artist, artist, album, album, title, titleField.text)
        }

        if (artist!==artistField.text || album!==albumField.text) {
            misdatos.clearList()
            if (lastGroup=="albums")
                misdatos.loadAlbums(utils.order)
            else if (lastGroup=="artists")
                misdatos.loadArtists()
            else
                misdatos.loadSongs(utils.readSettings("TrackOrder", "title"))
        }
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
                visible: meta.filename.indexOf(".mod")===-1 && meta.filename.indexOf(".xm")===-1
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
                EnterKey.onClicked: titleField.forceActiveFocus()
                visible: meta.filename.indexOf(".mod")===-1 && meta.filename.indexOf(".xm")===-1
            }

            TextField {
                id: titleField
                width: parent.width
                text: meta.title
                placeholderText: qsTr("Title")
                label: qsTr("Title")
                //focus: true
                EnterKey.enabled: titleField.text.length>0
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
                visible: meta.filename.indexOf(".mod")===-1 && meta.filename.indexOf(".xm")===-1
            }

        }

    }

}
